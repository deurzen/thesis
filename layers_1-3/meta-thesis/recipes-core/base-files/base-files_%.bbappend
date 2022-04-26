FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://root-profile \
"

do_install_append() {
    cat ${WORKDIR}/root-profile >> ${D}${ROOT_HOME}/.profile
    chmod 0644 ${D}${ROOT_HOME}/.profile
}

FILES_${PN} += " \
    ${ROOT_HOME}/.profile \
"
