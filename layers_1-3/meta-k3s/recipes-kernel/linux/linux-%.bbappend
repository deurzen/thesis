LINUX_MAJOR = "${@(d.getVar('LINUX_VERSION') or "x.y").split('.')[0]}"
LINUX_MINOR = "${@(d.getVar('LINUX_VERSION') or "x.y").split('.')[1]}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:${THISDIR}/files:"

SRC_URI_append = "\
    file://k3s.scc \
"

KERNEL_FEATURES_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'kvm', 'features/kvm/qemu-kvm-enable.scc', '', d)}"
KERNEL_MODULE_AUTOLOAD += "${@bb.utils.contains('DISTRO_FEATURES', 'kvm', 'kvm', '', d)}"

KERNEL_MODULE_AUTOLOAD += "nf_conntrack_ipv6 openvswitch"

KERNEL_FEATURES_append = " cfg/virtio.scc"
