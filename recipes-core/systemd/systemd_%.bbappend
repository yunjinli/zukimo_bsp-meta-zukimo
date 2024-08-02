
# Change preset to disable services that are not needed
do_configure:append() {
    # disable autostart of systemd-timesyncd as we do not want to use ntp
    sed -i -e "s/enable systemd-timesyncd.service/disable systemd-timesyncd.service/g" ${S}/presets/90-systemd.preset
}
