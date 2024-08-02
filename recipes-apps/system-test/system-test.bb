DESCRIPTION = "DCT System Test"
SECTION = "dct"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit cmake

DEPENDS += "googletest"
RDEPENDS:${PN} += "bash"

SRC_URI = "\
	file://CMakeLists.txt \
	file://main.cpp \
	file://system_test.cpp \
	file://system_test.hpp \
    file://start_system_test.sh \
"

S = "${WORKDIR}"

# do_install:append () {
# 	install -d ${HOME_ROOT}
# 	#install -m 755 ${S}/start_system_test.sh        ${HOME_ROOT}
# }

EXTRA_OECMAKE = ""

# FILES:${PN} += "                 \
#     /usr/bin/system_test       \
# "