# prepend to FILESEXTRAPATH to inject our own boot script.
# in addition to the minimal stuff the original boot script
# does, we want to mount the data partition read-writeable
# and prepare an overlayfs for /etc.
 
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"