#!/bin/sh

if [ ! -e /opt/cni/bin ]; then
    exit 1
fi

/bin/mv -f /var/lib/kubernetes-cni-plugins/* /opt/cni/bin/
/bin/rmdir /var/lib/kubernetes-cni-plugins
