FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://check-config.sh \
    file://k3s-server.sh \
    file://k3s-server-hostname-randomizer.sh \
    file://k3s-worker.sh \
    file://k3s-worker-hostname-randomizer.sh \
    file://k3s-server.service \
    file://k3s-server-hostname-randomizer.service \
    file://k3s-server-token.service \
    file://k3s-worker.service \
    file://k3s-worker-hostname-randomizer.service \
"

BIN_PREFIX ?= "${exec_prefix}/local"

do_install_append() {
    install -d ${D}${BIN_PREFIX}/bin

    install -D -m 0777 ${WORKDIR}/check-config.sh ${D}/home/root/check-config.sh

    install -D -m 0644 ${WORKDIR}/k3s-server.service ${D}${systemd_system_unitdir}/
    install -D -m 0644 ${WORKDIR}/k3s-server-hostname-randomizer.service ${D}${systemd_system_unitdir}/
    install -D -m 0644 ${WORKDIR}/k3s-server-token.service ${D}${systemd_system_unitdir}/

    install -D -m 0644 ${WORKDIR}/k3s-worker.service ${D}${systemd_system_unitdir}/
    install -D -m 0644 ${WORKDIR}/k3s-worker-hostname-randomizer.service ${D}${systemd_system_unitdir}/

    install -D -m 755 ${WORKDIR}/k3s-server.sh ${D}${BIN_PREFIX}/bin/
    install -D -m 755 ${WORKDIR}/k3s-server-hostname-randomizer.sh ${D}${BIN_PREFIX}/bin/
    install -D -m 755 ${WORKDIR}/k3s-worker.sh ${D}${BIN_PREFIX}/bin/
    install -D -m 755 ${WORKDIR}/k3s-worker-hostname-randomizer.sh ${D}${BIN_PREFIX}/bin/

    sed -i "s,K3S_SERVER_URI,${K3S_SERVER_URI},g" ${D}${BIN_PREFIX}/bin/k3s-worker.sh
    sed -i "s,K3S_SERVER_TOKEN,${K3S_SERVER_TOKEN},g" ${D}${BIN_PREFIX}/bin/k3s-worker.sh
}

PACKAGES =+ "k3s-server k3s-worker"

SYSTEMD_PACKAGES = "k3s-server k3s-worker"

SYSTEMD_SERVICE_k3s-server = "k3s-server.service k3s-server-hostname-randomizer.service"
SYSTEMD_SERVICE_k3s-worker = "k3s-worker.service k3s-worker-hostname-randomizer.service"
SYSTEMD_AUTO_ENABLE_k3s-server = "enable"
SYSTEMD_AUTO_ENABLE_k3s-worker = "enable"

RDEPENDS_${PN}_remove = " kubeadm kubectl kubelet cni"

RDEPENDS_k3s-server = "${PN}"
RDEPENDS_k3s-worker = "${PN}"

FILES_k3s-server = " \
    /home/root/check-config.sh \
    ${BIN_PREFIX}/bin/k3s-server.sh \
    ${BIN_PREFIX}/bin/k3s-server-hostname-randomizer.sh \
    ${systemd_system_unitdir}/k3s-server.service \
    ${systemd_system_unitdir}/k3s-server-hostname-randomizer.service \
    ${systemd_system_unitdir}/k3s-server-token.service \
"

FILES_k3s-worker += " \
    /home/root/check-config.sh \
    ${BIN_PREFIX}/bin/k3s-worker.sh \
    ${BIN_PREFIX}/bin/k3s-worker-hostname-randomizer.sh \
    ${systemd_system_unitdir}/k3s-worker.service \
    ${systemd_system_unitdir}/k3s-worker-hostname-randomizer.service \
"
