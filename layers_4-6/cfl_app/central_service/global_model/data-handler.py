#!/usr/bin/env python3

import torch
import torchvision.transforms as transforms
from torchvision.datasets import CIFAR10
from abc import ABCMeta, abstractmethod

class _DataHandler(metaclass=ABCMeta):
    @abstractmethod
    def __init__(self, batch_size, data_dir) -> None:
        """Initial data acquisition and preparation"""
        super(_DataHandler, self).__init__()
        pass

    @abstractmethod
    def next_batch(self):
        """Yield a batch of data (blocking)"""
        pass

    @abstractmethod
    def testing_data(self):
        """Iterate over testing data"""
        pass

class DataHandler(_DataHandler):
    def __init__(self, batch_size, data_dir) -> None:
        """Initial data acquisition and preparation"""
        super(DataHandler, self).__init__(batch_size, data_dir)

        transform = transforms.Compose([
            transforms.RandomCrop(32, padding=4),
            transforms.RandomHorizontalFlip(),
            transforms.ToTensor(),
            transforms.Normalize((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010)),
        ])

        trainset = CIFAR10(data_dir,
                           train=True,
                           download=True,
                           transform=transform)

        mask = range(0, len(trainset), 3)
        trainset = torch.utils.data.Subset(trainset, mask)

        transform = transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010)),
        ])

        testset = CIFAR10(data_dir,
                          train=False,
                          download=True,
                          transform=transform)

        self._trainloader = torch.utils.data.DataLoader(trainset,
                                                        batch_size=batch_size,
                                                        shuffle=True)

        self._testloader = torch.utils.data.DataLoader(testset,
                                                       batch_size=batch_size,
                                                        shuffle=False)

    def next_batch(self):
        """Yield a batch of data (blocking)"""
        return iter(self._trainloader)

    def testing_data(self):
        """Iterate over testing data"""
        return iter(self._testloader)
