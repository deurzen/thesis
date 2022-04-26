FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://root-profile \
"

#HOST_NAME = "`openssl rand -hex 5`"
#hostname = "${HOST_NAME}"

do_install_append() {
    cat ${WORKDIR}/root-profile >> ${D}${ROOT_HOME}/.profile
    chmod 0644 ${D}${ROOT_HOME}/.profile
}
