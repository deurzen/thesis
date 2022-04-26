#!/bin/sh

__set_hostname() {
  local hostname="a-$(/usr/bin/openssl rand -hex 5)"
  hostnamectl set-hostname "$hostname"
  sed -i "/127.0.1.1/s,^\([^ ]*\)\( *\)\([^ ]*\),\1\2${hostname},g" /etc/hosts
}

__set_hostname
