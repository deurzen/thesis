#!/usr/bin/env python3

import os
import random
import numpy as np
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

@app.route('/fallback', methods=['GET', 'POST'])
def fallback():
    if request.method == 'GET':
        return str(fl_app.falling_back)

    if request.method == 'POST':
        status = request.form['status']
        fl_app.instigate_fallback_mode(status)

    return 'Ok'

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
        commround = data['commround']

        with state_mutex:
            fl_app.receive_state_from_peer(id, commround, state)

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

class _DataHandler(metaclass=ABCMeta):
    @required(True)
    @abstractmethod
    def __init__(self, batch_size, data_dir, id):
        """Initial data acquisition and preparation"""
        super(_DataHandler, self).__init__()
        pass

    @required(True)
    @abstractmethod
    def get_training_data(self):
        """Yield training data (blocking)"""
        pass

    @required(True)
    @abstractmethod
    def get_testing_data(self):
        """Yield testing data"""
        pass

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
            and 'subscriber' in participant
            and 'snooper' in participant)

def _participant_component_registration_status(participant, component, color):
    component_status = '['
    components = {'publisher': '↑' if 'publisher' in participant else '·',
                  'subscriber': '↓' if 'subscriber' in participant else '·',
                  'snooper': '⟷' if 'snooper' in participant else '·'}
    components[component] = colored(components[component], color, attrs=['bold'])
    component_status += ' '.join(components.values())
    component_status += ']'
    return component_status

class FLApp:
    def __init__(self, commrounds, drift_threshold, epochs, batch_size, data_dir, seed, id):
        """Create training and test sets,
           split training set into initial and post hoc sets"""

        self.status = 'initializing'
        logging.debug(f'Initializing server with ID {{{id}}}')

        if seed is not None:
            np.random.seed(seed)
            random.seed(seed)

        self.id = id

        self.falling_back = False

        self._participants = {}
        self._registered_participants = {}
        self._incomplete_participants = {}

        self._model = Model(seed)
        self._data_handler = DataHandler(batch_size=batch_size,
                                         data_dir=data_dir)
        self._participant_model_states = {}
        self._unregistered_participant_model_states = {}

        self._commrounds = commrounds
        self._target_commround = None
        self.commround = 0
        self._commround_trained = False

        self.drift_theshold = drift_threshold
        self.corrected_drift = False

        self._epochs_per_commround = epochs
        self._batch_size = batch_size

        self.status = 'idling'

    def run(self):
        """Start main loop"""
        while self.commround <= self._commrounds:
            if self.commround not in self._participant_model_states:
                self._participant_model_states[self.commround] = {}
            if self.commround not in self._unregistered_participant_model_states:
                self._unregistered_participant_model_states[self.commround] = {}

            logging.debug(colored(f'Commencing R{self.commround + 1}', 'blue'))

            self.__train()
            self._commround_trained = True
            logging.debug(f'[L, R{self.commround + 1}] Test accuracy: {self.__test()}')

            if not self.falling_back and (self._target_commround is None or self._target_commround == self.commround):
                self._target_commround = None
                self.__communicate_states()
                if len(self._participants) > 0 and not self.corrected_drift:
                    logging.debug(colored(f'[F, R{self.commround + 1}] Test accuracy: {self.__test()}', 'yellow'))

            with state_mutex:
                self._admit_registered_participants()

            self.corrected_drift = False

            if self.commround in self._participant_model_states:
                del self._participant_model_states[self.commround]

            if self.commround in self._unregistered_participant_model_states:
                del self._unregistered_participant_model_states[self.commround]

            self.commround += 1

        logging.debug(colored('Training completed', 'green'))
        print(str(pickle.dumps(self.__save_state()).hex()))

    def receive_state_from_peer(self, id, commround, state):
        if id not in self._participants:
            logging.debug(colored(f'Received state that stems from unregistered peer {{{id}}}', 'red'))
            if commround >= self.commround:
                if commround not in self._unregistered_participant_model_states:
                    self._unregistered_participant_model_states[commround] = {}
                self._unregistered_participant_model_states[commround][id] = state
            return

        if commround > self.commround + self.drift_theshold:
            logging.debug(colored(f'Drift detected: syncing state with {{{id}}} at R{commround}', 'magenta'))
            if commround not in self._participant_model_states:
                self._participant_model_states[commround] = {}
            self._participant_model_states[commround][id] = state
            self.__load_state(state)
            self.commround = commround
            self.corrected_drift = True

        elif commround == self.commround:
            logging.debug(f'[{len(self._participant_model_states[commround]) + 1}/{len(self._participants)}] Received state from {{{id}}}')
            if commround not in self._participant_model_states:
                self._participant_model_states[commround] = {}
            self._participant_model_states[commround][id] = state

        elif commround > self.commround:
            self._target_commround = commround
            if commround not in self._participant_model_states:
                self._participant_model_states[commround] = {}
            self._participant_model_states[commround][id] = state


    def register_participant_component(self, id, role):
        if id in self._participants or id in self._registered_participants:
            return

        if id not in self._incomplete_participants:
            self._incomplete_participants[id] = set()

        self._incomplete_participants[id].add(role)
        logging.debug(f'Registered {_participant_component_registration_status(self._incomplete_participants[id], role, "green")} of {{{id}}}')

        if _participant_is_complete(self._incomplete_participants[id]):
            logging.debug(f'Registration of {{{id}}} completed, will participate after R{self.commround + 1}')
            self._registered_participants[id] = self._incomplete_participants[id]
            del self._incomplete_participants[id]

    def unregister_participant_component(self, id, role):
        if id in self._participants:
            logging.debug(colored(f'{{{id}}} is no longer a participant in the network', 'red'))
            self._incomplete_participants[id] = self._participants[id]
            del self._participants[id]

        if id in self._registered_participants:
            self._incomplete_participants[id] = self._registered_participants[id]
            del self._registered_participants[id]

        for commround in range(self.commround, self.commround + self.drift_theshold + 1):
            if commround in self._participant_model_states:
                if id in self._participant_model_states[commround]:
                    del self._participant_model_states[commround][id]

        if id not in self._incomplete_participants:
            return

        logging.debug(f'Unregistered {_participant_component_registration_status(self._incomplete_participants[id], role, "red")} of {{{id}}}')
        self._incomplete_participants[id].remove(role)

        if len(self._incomplete_participants[id]) == 0:
            for commround in range(self.commround, self.commround + self.drift_theshold + 1):
                if commround in self._unregistered_participant_model_states:
                    if id in self._unregistered_participant_model_states[commround]:
                        del self._unregistered_participant_model_states[commround][id]
            del self._incomplete_participants[id]

    def instigate_fallback_mode(self, status):
        was_falling_back = fl_app.falling_back
        fl_app.falling_back = status == 'on'

        if was_falling_back != fl_app.falling_back:
            if fl_app.falling_back:
                logging.debug(colored('Falling back to offline mode', 'cyan'))
            else:
                logging.debug(colored('Going back online', 'cyan'))

    def _admit_registered_participants(self):
        for id in self._registered_participants.keys():
            self._participants[id] = self._registered_participants[id]
            logging.debug(colored(f'{{{id}}} has now been acknowledged as a participant in the network', 'green'))

            to_delete = []
            max_commround = 0
            for commround in self._unregistered_participant_model_states.keys():
                if commround >= self.commround:
                    if id in self._unregistered_participant_model_states[commround]:
                        if commround not in self._participant_model_states:
                            self._participant_model_states[commround] = {}
                        if commround > self.commround + self.drift_theshold:
                            logging.debug(colored(f'Drift detected: syncing state with {{{id}}} at R{commround}', 'magenta'))
                            self.__load_state(self._unregistered_participant_model_states[commround][id])
                            self.commround = commround
                            self.corrected_drift = True
                        self._participant_model_states[commround][id] = self._unregistered_participant_model_states[commround][id]
                        del self._unregistered_participant_model_states[commround][id]

                    if commround > max_commround:
                        max_commround = commround
                else:
                    to_delete.append(commround)

            for commround in to_delete:
                del self._unregistered_participant_model_states[commround]

            if max_commround > self.commround and (self._target_commround is None or (max_commround > self._target_commround)):
                self._target_commround = max_commround

        self._registered_participants.clear()

    def predict(self, sample):
        """Predict the class of a single input sample"""
        sample = self._model.preprocess_sample(sample)
        with predict_mutex:
            return self._model.predict(sample)

    def __train(self):
        """Train the model"""
        for epoch in range(1, self._epochs_per_commround + 1):
            self.status = 'idling'
            with state_mutex:
                self.status = f'Training (commround: {self.commround + 1}, epoch: {epoch})'
                self._model.train(self._data_handler.get_training_data)

        self.status = 'idling'

    def __test(self):
        """Validate the model on the test set"""
        with state_mutex:
            self.status = 'testing'
            accuracy = self._model.test(self._data_handler.get_testing_data)
        self.status = 'idling'
        return accuracy

    def __save_state(self):
        return self._model.get_state()

    def __load_state(self, state):
        with predict_mutex:
            self._model.set_state(state)

    def __federate_states(self):
        self.status = 'generating federated model'
        federated_state = self.__save_state()

        if (self.falling_back or len(self._participants) == 0) or self._target_commround is not None:
            return

        states = [federated_state.copy()]
        for _, state in self._participant_model_states[self.commround].items():
            states.append(state)

        for key in federated_state:
            keyed_state_values = torch.stack(list(map(lambda x: x[key], states)))
            keyed_state_sum = torch.sum(keyed_state_values, dim=0)
            federated_state[key] = keyed_state_sum / float(len(self._participants) + 1)

        self.__load_state(federated_state)

        self._participant_model_states[self.commround].clear()
        self.status = 'idling'

    def __communicate_states(self):
        self.status = 'aggregating peer states'
        state = self.__save_state()
        commround = self.commround

        if self.corrected_drift or self._target_commround is not None:
            self.status = 'idling'
            return

        while not state_queue.empty():
            state_queue.get()(state, commround)
            state_queue.task_done()

        with state_mutex:
            ready = self.falling_back or len(self._participant_model_states[commround]) >= (len(self._participants))

        while not ready:
            time.sleep(0.2)

            if self._target_commround is not None:
                return

            while not state_queue.empty():
                state_queue.get()(state, commround)
                state_queue.task_done()

            with state_mutex:
                ready = self.falling_back or len(self._participant_model_states[commround]) >= (len(self._participants))

        self._commround_trained = False
        self.__federate_states()
        self.status = 'idling'


parser = argparse.ArgumentParser(description='FL app argument parser')
parser.add_argument('-c', '--commrounds', type=int, default=500, help='number of communication rounds to perform')
parser.add_argument('-D', '--drift-threshold', type=int, default=3, help='drift threshold: inherit state from peer if commround exceeds this value')
parser.add_argument('-e', '--epochs', type=int, default=1, help='number of epochs per communication round')
parser.add_argument('-b', '--batch-size', type=int, default=50, help='batch size')
parser.add_argument('-d', '--data-dir', type=str, default='./data/cifar-10', help='directory containing train/test data')
parser.add_argument('-s', '--seed', type=int, default=None, help='PyTorch seed to establish determinism')
parser.add_argument('-n', '--host', type=str, default='127.0.0.1', help='hostname the internal HTTP server binds to')
parser.add_argument('-p', '--port', type=int, default=38866, help='port behind which to open the internal HTTP server')
parser.add_argument('--model', type=str, default='model', help='name of (local) module providing the model')
parser.add_argument('--data-handler', type=str, default='data-handler', help='name of (local) module providing data handler')
parser.add_argument('--id',
                    type=lambda x: x if all(c in string.hexdigits for c in x[-10:]) and len(x) >= 10 else '%010X' % random.randrange(16**10),
                    default='%010X' % random.randrange(16**10),
                    help='the FL app\'s unique ID')
args = parser.parse_args()

data_handler_methods = (field for field in dir(_DataHandler) if hasattr(getattr(_DataHandler, field), 'required'))
model_methods = (field for field in dir(_Model) if hasattr(getattr(_Model, field), 'required'))

while True:
    try:
        DataHandler = _dynamic_import(args.data_handler + '.DataHandler')
        Model = _dynamic_import(args.model + '.Model')

        for method in data_handler_methods:
            assert(callable(getattr(DataHandler, method)))

        for method in model_methods:
            assert(callable(getattr(Model, method)))

        break
    except:
        logging.error('could not import local dependencies')
        time.sleep(0.5)

fl_app = FLApp(commrounds=args.commrounds,
               drift_threshold=args.drift_threshold,
               epochs=args.epochs,
               batch_size=args.batch_size,
               data_dir=args.data_dir,
               seed=args.seed,
               id=args.id[-10:])

if __name__ == '__main__':
    threading.Thread(target=fl_app.run).start()
    app.secret_key = 'f3d3r4t3d_l34rn1ng'
    app.run(threaded=True, host=args.host, port=args.port)
