#!/bin/sh

curl -sfL https://get.k3s.io | INSTALL_K3S_EXEC='--cluster-cidr=10.244.0.0/16' INSTALL_K3S_SYMLINK=force sh -s -
systemctl start k3s-server-token
