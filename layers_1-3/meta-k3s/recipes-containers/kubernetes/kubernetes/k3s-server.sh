#!/bin/sh

if [ -e /var/lib/k3s-install-args ]; then
    K3S_INSTALL_ARGS=$(cat /var/lib/k3s-install-args)
fi

curl -sfL https://get.k3s.io | eval ${K3S_INSTALL_ARGS:-""} INSTALL_K3S_SYMLINK=force sh -s -
systemctl start k3s-server-token
