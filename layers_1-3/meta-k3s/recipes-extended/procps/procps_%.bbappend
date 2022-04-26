FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://99-kubernetes-cri.conf \
"

do_install_append() {
    install -d ${D}${sysconfdir}/sysctl.d
    install -m 0644 ${WORKDIR}/99-kubernetes-cri.conf ${D}${sysconfdir}/sysctl.d/
}

FILES_${PN} += " \
    ${sysconfdir}/sysctl.d/99-kubernetes-cri.conf \
"
