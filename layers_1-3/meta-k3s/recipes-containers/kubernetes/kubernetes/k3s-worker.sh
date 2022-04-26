#!/bin/sh

curl -sfL https://get.k3s.io | K3S_URL=https://K3S_SERVER_URI:6443 K3S_TOKEN=K3S_SERVER_TOKEN INSTALL_K3S_SYMLINK=force sh -s -
