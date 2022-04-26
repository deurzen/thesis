IMAGE_ROOTFS_EXTRA_SPACE = "10000000"

IMAGE_INSTALL:append = " iproute2 iptables ebtables ca-certificates"
IMAGE_INSTALL:append = " libnetfilter-conntrack conntrack-tools ethtool"
IMAGE_INSTALL:append = " ntp curl"
IMAGE_INSTALL:append = " kubernetes"

IMAGE_FEATURES:append = " ssh-server-dropbear"

IMAGE_FEATURES_remove = " psplash"

NOHDD="1"
NOISO="1"
