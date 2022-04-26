#!/usr/bin/env python3

import torch
import torchvision.transforms as transforms
import torch.nn as nn
import torch.nn.functional as F
from abc import ABCMeta, abstractmethod

class _Model(metaclass=ABCMeta):
    @abstractmethod
    def __init__(self):
        """Define (initialize) the model"""
        super(_Model, self).__init__()
        pass

    @abstractmethod
    def preprocess_sample(self, sample):
        """Preprocess a single raw input sample"""
        pass

    @abstractmethod
    def train(self, get_training_data):
        """Train a single epoch"""
        pass

    @abstractmethod
    def test(self, get_testing_data):
        """Evaluate the model"""
        pass

    @abstractmethod
    def predict(self, sample):
        """Predict the class of a single input sample"""
        pass

    @abstractmethod
    def get_state(self):
        """Should return the model's state (parameters)"""
        pass

    @abstractmethod
    def set_state(self, state):
        """Set the model's state (parameters)"""
        pass

class Model(_Model, nn.Module):
    def __init__(self, seed):
        """Define (initialize) the network"""
        super(Model, self).__init__()

        if seed is not None:
            torch.manual_seed(seed)
            torch.cuda.manual_seed(seed)

        # define class names
        self._classes = ('plane', 'car', 'bird', 'cat', 'deer',
            'dog', 'frog', 'horse', 'ship', 'truck')

        # initialize model building blocks
        self.conv1 = nn.Conv2d(3, 6, 5)
        self.pool = nn.MaxPool2d(2, 2)
        self.conv2 = nn.Conv2d(6, 16, 5)
        self.fc1 = nn.Linear(16 * 5 * 5, 120)
        self.fc2 = nn.Linear(120, 84)
        self.fc3 = nn.Linear(84, 10)

        # define loss and optimizer
        self._criterion = nn.CrossEntropyLoss()
        self._optimizer = torch.optim.SGD(self.parameters(),
                                          lr=0.003,
                                          momentum=0.89)

    def forward(self, x):
        """Define the forward step"""
        x = self.pool(F.relu(self.conv1(x)))
        x = self.pool(F.relu(self.conv2(x)))
        x = x.view(-1, 16 * 5 * 5)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)

        return x

    def preprocess_sample(self, sample):
        """Preprocess a single raw input sample"""
        transform = transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010)),
        ])

        return transform(sample)

    def train(self, get_training_data):
        """Train a single epoch"""
        state = torch.get_rng_state()
        for batch in get_training_data():
            images, labels = batch[0], batch[1]

            # zero the parameter gradients
            self._optimizer.zero_grad()

            # forward + backward + optimize
            outputs = self(images)
            loss = self._criterion(outputs, labels)
            loss.backward()
            self._optimizer.step()
        torch.set_rng_state(state)

    def test(self, get_testing_data):
        """Evaluate the model"""
        loss = 0.0
        correct = 0
        total = 0

        state = torch.get_rng_state()
        for data_ in get_testing_data():
            with torch.no_grad():
                images, labels = data_[0], data_[1]
                outputs = self(images)

                loss += self._criterion(outputs, labels).item()
                _, predicted = torch.max(outputs.data, 1)

                total += labels.size(0)
                correct += (predicted == labels).sum().item()
        torch.set_rng_state(state)

        return correct / total

    def predict(self, sample):
        """Predict the class of a single input sample"""
        output = self(sample.reshape(1, 3, 32, 32))
        _, predicted = torch.max(output, 1)
        return self._classes[predicted[0]]

    def get_state(self):
        return self.state_dict()

    def set_state(self, state):
        self.load_state_dict(state)
