SUMMARY = "Weave CNI layer"
DESCRIPTION = "Weave CNI overlay"

LICENSE = "LGPLv3"
LIC_FILES_CHKSUM = "file://${WORKDIR}/COPYING.LGPL-3;md5=ed435b02f8ca1d6fbef9f37e7528ad07"

SRCREV_plugins = "1f33fb729ae2b8900785f896df2dc1f6fe5e8239"
SRC_URI = " \
    https://raw.githubusercontent.com/weaveworks/weave/master/COPYING.LGPL-3;md5sum=ed435b02f8ca1d6fbef9f37e7528ad07 \
    file://weave.yml;md5sum=e8931444b0807c41559501a00b0b7c6e \
    git://github.com/containernetworking/plugins.git;nobranch=1;destsuffix=${S}/src/github.com/containernetworking/plugins;name=plugins;protocol=https \
    file://cni-plugins-install.service;md5sum=3b72ea6c66e0d07e941dd5f0c1fbd482 \
    file://install-cni-plugins.sh;md5sum=33da06f56bfc3f2ece23a5578b386a11 \
"

K3S_INSTALL_ARGS = " \
    INSTALL_K3S_EXEC='--flannel-backend=none' \
"

BIN_PREFIX ?= "${exec_prefix}/local"

GO_IMPORT = "import"

inherit systemd
inherit go
inherit goarch

do_compile() {
	cd ${B}/src/github.com/containernetworking/plugins
	PLUGINS="$(ls -d plugins/meta/*; ls -d plugins/ipam/*; ls -d plugins/main/* | grep -v windows)"
	mkdir -p ${B}/plugins/bin/
	for p in $PLUGINS; do
	    plugin="$(basename "$p")"
	    echo "building: $p"
	    ${GO} build -mod=vendor -o ${B}/plugins/bin/$plugin github.com/containernetworking/plugins/$p
	done
}

do_install() {
    install -d ${D}${BIN_PREFIX}/bin
    install -D -m 755 ${WORKDIR}/install-cni-plugins.sh ${D}${BIN_PREFIX}/bin/

    install -d ${D}/var/lib/
    echo "${K3S_INSTALL_ARGS}" >| ${D}/var/lib/k3s-install-args

    install -d ${D}/var/lib/kubernetes-cni-plugins
    install -m 755 -D ${B}/plugins/bin/* ${D}/var/lib/kubernetes-cni-plugins

    install -d ${D}/var/lib/rancher/k3s/server/manifests
    install -D -m 0644 ${WORKDIR}/weave.yml ${D}/var/lib/rancher/k3s/server/manifests/

    install -d ${D}${systemd_system_unitdir}
    install -D -m 0644 ${WORKDIR}/cni-plugins-install.service ${D}${systemd_system_unitdir}/
}

SYSTEMD_SERVICE_${PN} = "cni-plugins-install.service"
SYSTEMD_AUTO_ENABLE_${PN} = "enable"

FILES_${PN} = " \
    /var/lib/k3s-install-args \
    /var/lib/kubernetes-cni-plugins \
    /var/lib/rancher/k3s/server/manifests/ \
    /var/lib/rancher/k3s/server/manifests/weave.yml \
    ${systemd_system_unitdir}/cni-plugins-install.service \
    ${BIN_PREFIX}/bin/install-cni-plugins.sh \
"

INSANE_SKIP_${PN} += "ldflags already-stripped"

deltask compile_ptest_base
