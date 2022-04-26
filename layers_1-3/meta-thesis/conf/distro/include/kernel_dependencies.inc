KERNEL_MODULE_AUTOLOAD += " \
    br_netfilter \
    "

IMAGE_INSTALL:append = " \
    kernel-module-xt-addrtype \
    kernel-module-xt-nat \
    kernel-module-xt-multiport \
    kernel-module-xt-conntrack \
    kernel-module-xt-comment \
    kernel-module-xt-mark \
    kernel-module-xt-connmark \
    kernel-module-vxlan \
    kernel-module-xt-masquerade \
"
