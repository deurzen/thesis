#!/usr/bin/env python3

import os
import random
import argparse
import string
import pickle
import time
import threading
import logging
import torch
import threading
from termcolor import colored
from PIL import Image
from flask import Flask, request, flash, redirect, render_template_string
from werkzeug.utils import secure_filename
from abc import ABCMeta, abstractmethod
from queue import Queue

logging.basicConfig(level=logging.DEBUG,
                    format='%(message)s',)

state_mutex = threading.Lock()
predict_mutex = threading.Lock()
state_queue = Queue(maxsize=0)
app = Flask(__name__)

logging.getLogger('werkzeug').disabled = True
logging.getLogger('requests').disabled = True
logging.getLogger('urllib3.util.retry').disabled = True
logging.getLogger('urllib3.util').disabled = True
logging.getLogger('urllib3').disabled = True
logging.getLogger('urllib3.connection').disabled = True
logging.getLogger('urllib3.response').disabled = True
logging.getLogger('urllib3.connectionpool').disabled = True
logging.getLogger('urllib3.poolmanager').disabled = True
logging.getLogger('urllib3').disabled = True
os.environ['WERKZEUG_RUN_MAIN'] = 'true'

fl_app = None

@app.route('/', methods=['GET'])
def root():
    return f'FL app {fl_app.id}'

@app.route('/status', methods=['GET'])
def status():
    return fl_app.status

@app.route('/id', methods=['GET'])
def id():
    return str(fl_app.id)

@app.route('/commround', methods=['GET'])
def commround():
    return str(fl_app.commround)

@app.route('/participation', methods=['GET', 'POST'])
def participation():
    if request.method == 'GET':
        return str(len(fl_app._participants) + 1)

    if request.method == 'POST':
        id = request.form['id']
        role = request.form['role']
        action = request.form['action']

        if action == 'add':
            with state_mutex:
                fl_app.register_participant_component(id, role)

        elif action == 'remove':
            with state_mutex:
                fl_app.unregister_participant_component(id, role)

        return 'Ok'

@app.route('/states', methods=['GET'])
def states():
    return str(len(fl_app._participant_model_states[fl_app.commround]) + 1)

@app.route('/state', methods=['GET', 'POST'])
def state():
    if request.method == 'GET':
        task_res = {}
        def serve_request(state, commround):
            task_res['state'] = state
            task_res['commround'] = commround

        state_queue.put(serve_request)
        state_queue.join()

        return str(task_res['commround']) + str(':') + str(pickle.dumps(task_res).hex())

    if request.method == 'POST':
        id = request.form['id']
        data = pickle.loads(bytes.fromhex(request.form['state']))

        state = data['state']

        with state_mutex:
            fl_app.receive_state_from_peer(id, state)

        return 'Ok'

@app.route('/predict', methods=['GET', 'POST'])
def predict():
    UPLOAD_FOLDER = './data/uploads'

    if request.method == 'GET':
        return render_template_string('''
        <!doctype html>
        <title>Image classification</title>
        <body>
            <div style="width: 350px; margin: auto">
                <h1>Upload image to classify</h1>
                <form method=post enctype=multipart/form-data>
                  <input type=file name=file>
                  <input type=submit value=Upload>
                </form>
                {% with messages = get_flashed_messages() %}
                  {% if messages %}
                    <ul class=flashes>
                    {% for message in messages %}
                      <li>{{ message }}</li>
                    {% endfor %}
                    </ul>
                  {% endif %}
                {% endwith %}
                {% block body %}{% endblock %}
            </div>
        </body>
        ''')

    if request.method == 'POST':
        if 'file' not in request.files:
            flash('no file supplied')
            return redirect(request.url)

        file = request.files['file']

        if file and file.filename:
            filename = secure_filename(file.filename)
            filepath = os.path.join(UPLOAD_FOLDER, filename)
            file.save(filepath)

            prediction = fl_app.predict(Image.open(filepath))

            try:
                os.remove(filepath)
            except OSError:
                pass

            flash(prediction)
            return redirect(request.url)
        else:
            flash('no file supplied')
            return redirect(request.url)


def required(tag):
  def wrapper(method):
    method.required = tag
    return method
  return wrapper

class _Model(metaclass=ABCMeta):
    @required(True)
    @abstractmethod
    def __init__(self):
        """Define (initialize) the model"""
        super(_Model, self).__init__()
        pass

    @required(True)
    @abstractmethod
    def preprocess_sample(self, sample):
        """Preprocess a single raw input sample"""
        pass

    @required(True)
    @abstractmethod
    def train(self, get_training_data):
        """Train a single epoch"""
        pass

    @required(True)
    @abstractmethod
    def test(self, get_testing_data):
        """Evaluate the model"""
        pass

    @required(True)
    @abstractmethod
    def predict(self, sample):
        """Predict the class of a single input sample"""
        pass

    @required(True)
    @abstractmethod
    def get_state(self):
        """Should return the model's state (parameters)"""
        pass

    @required(True)
    @abstractmethod
    def set_state(self, state):
        """Set the model's state (parameters)"""
        pass


def _dynamic_import(name):
    components = name.split('.')
    module = __import__(components[0])

    for component in components[1:]:
        module = getattr(module, component)

    return module


def _participant_is_complete(participant):
    return ('publisher' in participant
            and 'subscriber' in participant)

def _participant_component_registration_status(participant, component, color):
    component_status = '['
    components = {'publisher': '↑' if 'publisher' in participant else '·',
                  'subscriber': '↓' if 'subscriber' in participant else '·'}
    components[component] = colored(components[component], color, attrs=['bold'])
    component_status += ' '.join(components.values())
    component_status += ']'
    return component_status

class FLApp:
    def __init__(self, commrounds, seed, id):
        """Create training and test sets,
           split training set into initial and post hoc sets"""

        self.status = 'initializing'
        logging.debug(f'Initializing server with ID {{{id}}}')

        self.id = id

        self._participants = {}
        self._registered_participants = {}
        self._incomplete_participants = {}

        self._model = Model(seed)
        self._participant_model_states = {}

        self._commrounds = commrounds
        self.commround = 0

        self._initializing = True
        self.status = 'idling'

    def run(self):
        """Start main loop"""
        while self.commround <= self._commrounds:
            while self._initializing:
                while not state_queue.empty():
                    state_queue.get()(self.__save_state(), self.commround)
                    state_queue.task_done()
                    self._initializing = False
                time.sleep(1)

            with state_mutex:
                has_participants = (len(self._participants)) > 1

            # wait until all participants' states arrive
            while not has_participants:
                time.sleep(1)

                with state_mutex:
                    has_participants = (len(self._participants)) > 1

            logging.debug(colored(f'Accumulating R{self.commround + 1}', 'blue'))

            self.__communicate_states()

            self.commround += 1

        logging.debug(colored('Training completed', 'green'))

    def receive_state_from_peer(self, id, state):
        if id not in self._participants:
            logging.debug(colored(f'Received state that stems from unregistered peer {{{id}}}', 'red'))
            return

        if id not in self._participant_model_states:
            logging.debug(f'[{len(self._participant_model_states) + 1}/{len(self._participants)}] Received state from {{{id}}}')

        self._participant_model_states[id] = state


    def register_participant_component(self, id, role):
        if id in self._participants or id in self._registered_participants:
            return

        if id not in self._incomplete_participants:
            self._incomplete_participants[id] = set()

        self._incomplete_participants[id].add(role)
        logging.debug(f'Registered {_participant_component_registration_status(self._incomplete_participants[id], role, "green")} of {{{id}}}')

        if _participant_is_complete(self._incomplete_participants[id]):
            logging.debug(f'Registration of {{{id}}} completed')
            self._registered_participants[id] = self._incomplete_participants[id]
            del self._incomplete_participants[id]
            self._admit_registered_participants()

    def unregister_participant_component(self, id, role):
        if id in self._participants:
            logging.debug(colored(f'{{{id}}} is no longer a participant in the network', 'red'))
            self._incomplete_participants[id] = self._participants[id]
            del self._participants[id]

        if id in self._registered_participants:
            self._incomplete_participants[id] = self._registered_participants[id]
            del self._registered_participants[id]

        if id in self._participant_model_states:
            del self._participant_model_states[id]

        if id not in self._incomplete_participants:
            return

        logging.debug(f'Unregistered {_participant_component_registration_status(self._incomplete_participants[id], role, "red")} of {{{id}}}')
        self._incomplete_participants[id].remove(role)

        if len(self._incomplete_participants[id]) == 0:
            del self._incomplete_participants[id]

    def _admit_registered_participants(self):
        for id in self._registered_participants.keys():
            self._participants[id] = self._registered_participants[id]
            logging.debug(colored(f'{{{id}}} has now been acknowledged as a participant in the network', 'green'))

        self._registered_participants.clear()

    def predict(self, sample):
        """Predict the class of a single input sample"""
        sample = self._model.preprocess_sample(sample)
        with predict_mutex:
            return self._model.predict(sample)

    def __save_state(self):
        return self._model.get_state()

    def __load_state(self, state):
        with predict_mutex:
            self._model.set_state(state)

    def __federate_states(self):
        self.status = 'generating federated model'

        if (len(self._participants) <= 1):
            return

        federated_state = self.__save_state()
        states = [federated_state.copy()]
        for _, state in self._participant_model_states.items():
            states.append(state)

        # FedAvg
        for key in federated_state:
            keyed_state_values = torch.stack(list(map(lambda x: x[key], states)))
            keyed_state_sum = torch.sum(keyed_state_values, dim=0)
            federated_state[key] = keyed_state_sum / float(len(self._participants) + 1)

        self.__load_state(federated_state)

        self._participant_model_states.clear()
        self.status = 'idling'

    def __communicate_states(self):
        self.status = 'aggregating peer states'
        commround = self.commround

        with state_mutex:
            ready = len(self._participant_model_states) >= (len(self._participants))

        # wait until all participants' states arrive
        while not ready:
            time.sleep(1)

            with state_mutex:
                ready = len(self._participant_model_states) >= (len(self._participants))

        self.__federate_states()

        # push federated state back to participants
        state = self.__save_state()
        while not state_queue.empty():
            state_queue.get()(state, commround)
            state_queue.task_done()

        self.status = 'idling'


parser = argparse.ArgumentParser(description='FL app argument parser')
parser.add_argument('-c', '--commrounds', type=int, default=500, help='number of communication rounds to perform')
parser.add_argument('-s', '--seed', type=int, default=None, help='PyTorch seed to establish determinism')
parser.add_argument('-n', '--host', type=str, default='127.0.0.1', help='hostname the internal HTTP server binds to')
parser.add_argument('-p', '--port', type=int, default=38866, help='port behind which to open the internal HTTP server')
parser.add_argument('--model', type=str, default='model', help='name of (local) module providing the model')
parser.add_argument('--id',
                    type=lambda x: x if all(c in string.hexdigits for c in x[-10:]) and len(x) >= 10 else '%010X' % random.randrange(16**10),
                    default='%010X' % random.randrange(16**10),
                    help='the FL app\'s unique ID')
args = parser.parse_args()

model_methods = (field for field in dir(_Model) if hasattr(getattr(_Model, field), 'required'))

while True:
    try:
        Model = _dynamic_import(args.model + '.Model')

        for method in model_methods:
            assert(callable(getattr(Model, method)))

        break
    except:
        logging.error('could not import local dependencies')
        time.sleep(0.5)

fl_app = FLApp(commrounds=args.commrounds,
               seed=args.seed,
               id=args.id[-10:])

if __name__ == '__main__':
    threading.Thread(target=fl_app.run).start()
    app.secret_key = 'f3d3r4t3d_l34rn1ng'
    app.run(threaded=True, host=args.host, port=args.port)
