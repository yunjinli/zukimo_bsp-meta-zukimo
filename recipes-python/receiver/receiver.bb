SUMMARY = "Python socket receiver program"
DESCRIPTION = "A Python program to receive data over a socket and process it."
LICENSE = "CLOSED"
# LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=<checksum>"

SRC_URI = "file://receiver.py"
#SRC_URI[md5sum] = "49b32f7fd70c642b5e46cec15cd205f1"
#SRC_URI[sha256sum] = "c603c8a420fe5ddbe359bccd0c92c7bea79c9c0ca38eed19166a4fa2958cd339"

# Use Python 3 from the system
RDEPENDS_${PN} = "python3 python3-pickle"

# Install the script in the /usr/bin directory of the target filesystem
#do_install() {
#    install -d ${D}${bindir}
#    install -m 0755 ${WORKDIR}/receiver.py ${D}${bindir}/receiver.py
#}
do_install() {
    install -d ${D}/home/root
    install -m 0755 ${WORKDIR}/receiver.py ${D}/home/root/receiver.py
}

FILES:${PN} +=  "   \
    /home           \
    /home/*         \
"

