LICENSE = "CLOSED"
inherit systemd

SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_SERVICE:${PN} = "ptp4l.service phc2sys.service"

# Auto enable test_video.sevice only for demo build
#SYSTEMD_SERVICE:${PN} = " test_video.service"

SRC_URI:append = " \
  file://test_video.service \
  file://ptp4l.service \
  file://phc2sys.service \
"

do_install:append() {
  install -d ${D}/${systemd_unitdir}/system
  install -m 0644 ${WORKDIR}/test_video.service ${D}/${systemd_unitdir}/system
  install -m 0644 ${WORKDIR}/ptp4l.service ${D}/${systemd_unitdir}/system
  install -m 0644 ${WORKDIR}/phc2sys.service ${D}/${systemd_unitdir}/system
}

FILES:${PN} += "${systemd_unitdir}/system/test_video.service"
FILES:${PN} += "${systemd_unitdir}/system/ptp4l.service"
FILES:${PN} += "${systemd_unitdir}/system/phc2sys.service"