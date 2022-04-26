SUMMARY = "Flannel CNI layer"
DESCRIPTION = "Flannel CNI overlay"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"

SRC_URI = " \
    https://raw.githubusercontent.com/flannel-io/flannel/master/LICENSE;md5sum=3b83ef96387f14655fc854ddc3c6bd57 \
	https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml;md5sum=ada8bfd31f7ac7906bad33236c0b2dd0 \
"

K3S_INSTALL_ARGS = " \
    INSTALL_K3S_EXEC='--cluster-cidr=10.244.0.0/16' \
"

do_install() {
    install -d ${D}/var/lib/
    echo "${K3S_INSTALL_ARGS}" >| ${D}/var/lib/k3s-install-args

    install -d ${D}/var/lib/rancher/k3s/server/manifests
    install -D -m 0644 ${WORKDIR}/kube-flannel.yml ${D}/var/lib/rancher/k3s/server/manifests/
}

RDEPENDS_${PN} = "cni"

FILES_${PN} = " \
    /var/lib/k3s-install-args \
    /var/lib/rancher/k3s/server/manifests/ \
    /var/lib/rancher/k3s/server/manifests/kube-flannel.yml \
"
