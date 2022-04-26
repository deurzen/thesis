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
        data = pickle.loads(bytes.fromhex(request.form['state']))

        state = data['state']
        commround = data['commround']

        with state_mutex:
            fl_app.receive_global_state(commround, state)

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


class FLApp:
    def __init__(self, commrounds, epochs, batch_size, data_dir, seed, id):
        """Create training and test sets,
           split training set into initial and post hoc sets"""

        self.status = 'initializing'
        logging.debug(f'Initializing server with ID {{{id}}}')

        self.id = id

        self.falling_back = False

        self._model = Model(seed)
        self._data_handler = DataHandler(batch_size=batch_size,
                                         data_dir=data_dir)

        self._commrounds = commrounds
        self.commround = 0

        self._epochs_per_commround = epochs
        self._batch_size = batch_size

        self._global_states = {}

        self._initializing = True
        self.status = 'idling'

    def run(self):
        """Start main loop"""
        while self.commround <= self._commrounds:
            while self._initializing and self.commround not in self._global_states:
                time.sleep(1)

            if self._initializing:
                self.__load_state(self._global_states[self.commround])
                self._initializing = False

            logging.debug(colored(f'Commencing R{self.commround + 1}', 'blue'))

            self.__train()
            logging.debug(f'[L, R{self.commround + 1}] Test accuracy: {self.__test()}')

            if not self.falling_back:
                self.__communicate_state()
                logging.debug(colored(f'[F, R{self.commround + 1}] Test accuracy: {self.__test()}', 'yellow'))

            self.commround += 1

        logging.debug(colored('Training completed', 'green'))

    def receive_global_state(self, commround, state):
        if commround < self.commround:
            return

        if commround not in self._global_states:
            self._global_states = {}

        self._global_states[commround] = state

        if commround > self.commround:
            for i in range(self.commround, commround):
                if i in self._global_states:
                    print('DELETING', i)
                    del self._global_states[i]
            self.commround = commround

    def instigate_fallback_mode(self, status):
        was_falling_back = fl_app.falling_back
        fl_app.falling_back = status == 'on'

        if was_falling_back != fl_app.falling_back:
            if fl_app.falling_back:
                logging.debug(colored('Falling back to offline mode', 'cyan'))
            else:
                logging.debug(colored('Going back online', 'cyan'))

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

    def __communicate_state(self):
        self.status = 'communicating out local state'
        state = self.__save_state()
        commround = self.commround

        while not state_queue.empty():
            state_queue.get()(state, commround)
            state_queue.task_done()

        self.status = 'waiting for global state'

        with state_mutex:
            ready = self.falling_back or (self.commround in self._global_states)

        while not ready:
            time.sleep(0.5)

            while not state_queue.empty():
                state_queue.get()(state, commround)
                state_queue.task_done()

            with state_mutex:
                ready = self.falling_back or (self.commround in self._global_states)

        self.__load_state(self._global_states[self.commround])
        self.status = 'idling'


parser = argparse.ArgumentParser(description='FL app argument parser')
parser.add_argument('-c', '--commrounds', type=int, default=500, help='number of communication rounds to perform')
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
               epochs=args.epochs,
               batch_size=args.batch_size,
               data_dir=args.data_dir,
               seed=args.seed,
               id=args.id[-10:])

if __name__ == '__main__':
    threading.Thread(target=fl_app.run).start()
    app.secret_key = 'f3d3r4t3d_l34rn1ng'
    app.run(threaded=True, host=args.host, port=args.port)
