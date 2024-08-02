#!/bin/sh

# Multiple test pipelines that use gstreamer to process image data.
#
# For the network tests start a receiving gstreamer on the host device:
# isp0: gst-launch-1.0 udpsrc port=9078 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjitterbuffer ! rtpjpegdepay ! jpegdec ! autovideosink
# isp1: gst-launch-1.0 udpsrc port=9080 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjitterbuffer ! rtpjpegdepay ! jpegdec ! autovideosink

CMD=$1
PARAM=$2

show_commands() {
    echo "Simply select number of test (default: 2)"
    echo "1 - Simple test with v4l2-ctl"
    echo "2 - Start full pipeline isp 0: isp_pre->isp_main->mvdu"
    echo "3 - Start full pipeline isp 0: isp_pre->isp_main->nna->mvd"
    echo "4 - Start net pipeline  isp 0 port 9078: isp_pre->isp_main->network"
    echo "5 - Start net pipeline  isp 1 port 9080: isp_pre->isp_main->network"
    echo "6 - Start net pipeline  isp 0 port 9078: isp_pre->isp_main->nna->network"
    echo "7 - Start MVDU only test (VID + PIP)"
    echo "8 - Start combined NNA / MVDU test"
    echo "9 - Start camera 0 at isp0 + camera 3 at isp1 (pip)"
    echo "10- Start camera 0 at isp0 + nna + camera 3 at isp1 (pip)"
    echo "11- Start MVDU only test (VID + OSD)"
    echo "12- Start MVDU only test (OSD)"
    echo "13- Start NNA overlay pipeline"
    echo ""
    echo "-d  as second argument (or first in case of default) disables the start of the 3A tool"
    exit
}

config_mvdu() {
    v4l2-ctl --device /dev/video/mvdu1 --set-selection-output target=crop,top=0,left=0,width=640,height=480
    v4l2-ctl --device /dev/video/mvdu1 --set-selection-output target=compose,top=300,left=640,width=640,height=480
    v4l2-ctl --device /dev/video/mvdu1 --set-selection-output target=compose_default,top=300,left=640,width=640,height=480

    v4l2-ctl --device /dev/video/mvdu0 --set-dv-bt-timings index=12
    v4l2-ctl --device /dev/video/mvdu1 --set-dv-bt-timings index=12
    v4l2-ctl --device /dev/video/mvdu2 --set-dv-bt-timings index=12

    v4l2-ctl -d /dev/video/mvdu0 --set-ctrl bgcolor=0x808080
    v4l2-ctl -d /dev/video/mvdu1 --set-ctrl alpha=255
    v4l2-ctl -d /dev/video/mvdu2 --set-ctrl alpha=128
}

stats() {
    PIPE=$1
    if test -z $1 ; then
        PIPE=0
    fi
    if test $PIPE -eq 0 ; then
        PRE="26400200"
        MAIN="26000000"
        MAIN_DMA="26040000"
    fi
    if test $PIPE -eq 1 ; then
        PRE="26402200"
        MAIN="26080000"
        MAIN_DMA="260C0000"
    fi
    echo "#######################################"
    echo "# stats"
    echo "#######################################"
    cat /sys/devices/platform/${PRE}.isp_pre/debug_irqinfo
    cat /sys/devices/platform/${PRE}.isp_pre/clear_irqinfo
    cat /sys/devices/platform/isp_main_dma@${MAIN_DMA}/debug_dma_stats
    cat /sys/devices/platform/${MAIN}.isp_main/debug_irqinfo
}

start_3a() {
    dct-v4l2-3a -i /dev/video/isp0-hist -o /dev/video/isp0 -l &
    dct-v4l2-3a -i /dev/video/isp1-hist -o /dev/video/isp1 -l &
}

if test -z $CMD ; then
    CMD="2"
fi

if test -z $PARAM ; then
    PARAM="none"
fi

if test $CMD = "-h" || test $CMD = "--help" ; then
    show_commands
fi

echo "$CMD, $PARAM"
if test $CMD != "-d" && test $PARAM != "-d" ; then
    echo "Starting 3A for isp0 and isp1"
    start_3a

fi

if test $CMD -eq 1 ; then
    echo "start simple test of the video interface with v4l2-ctl"
    v4l2-ctl --stream-mmap --stream-count=100 -d /dev/video/vi0
    stats
fi

if test $CMD -eq 2 ; then
    echo "Start full pipeline: isp_pre->isp_main->mvdu"
    config_mvdu
    gst-launch-1.0 -e v4l2src device=/dev/video/isp0-hv ! "video/x-raw,format=NV61,width=1920,height=1080" ! v4l2sink device=/dev/video/mvdu0
    stats
fi

if test $CMD -eq 3 ; then
    echo "Start full pipeline: isp_pre->isp_main->nna->mvdu"
    config_mvdu
    gst-launch-1.0 -e v4l2src device=/dev/video/isp0-hv ! "video/x-raw,format=RGB,width=1920,height=1080,framerate=15/1" ! queue leaky=downstream ! v4l2convert disable-passthrough=true capture-io-mode=4 output-io-mode=4 ! v4l2sink device=/dev/video/mvdu0
    stats
fi

if test $CMD -eq 4 ; then
    echo "Start net pipeline isp 0: isp_pre->isp_main->network"
    gst-launch-1.0 -e v4l2src device=/dev/video/isp0-hv ! "video/x-raw,format=RGB,width=1920,height=1080" ! queue leaky=downstream ! videoconvertscale ! 'video/x-raw, width=640, height=480, format=(string)I420' ! jpegenc ! rtpjpegpay ! udpsink host=192.168.1.1 port=9078
    stats
fi

if test $CMD -eq 5 ; then
    echo "Start net pipeline isp 1: isp_pre->isp_main->network"
    gst-launch-1.0 -e v4l2src device=/dev/video/isp1-hv ! "video/x-raw,format=RGB,width=1920,height=1080" ! queue leaky=downstream ! videoconvertscale ! 'video/x-raw, width=640, height=480, format=(string)I420' ! jpegenc ! rtpjpegpay ! udpsink host=192.168.1.1 port=9080
    stats 1
fi

if test $CMD -eq 6 ; then
    echo "Start net pipeline isp 0: isp_pre->isp_main->nna->network"
    gst-launch-1.0 -e v4l2src device=/dev/video/isp0-hv ! "video/x-raw,format=RGB,width=1920,height=1080,framerate=15/1" ! queue leaky=downstream ! v4l2convert disable-passthrough=true capture-io-mode=4 output-io-mode=4 ! "video/x-raw,format=NV61,width=1920,height=1080" ! queue leaky=downstream ! videoconvertscale ! 'video/x-raw, width=640, height=480, format=(string)I420' ! jpegenc ! rtpjpegpay ! udpsink host=192.168.1.1 port=9078
    stats
fi

if test $CMD -eq 7 ; then
    echo "Start MVDU only test (VID + PIP)"
    config_mvdu
    gst-launch-1.0 videotestsrc ! "video/x-raw,format=NV61,width=640,height=480,framerate=30/1" ! v4l2sink device=/dev/video/mvdu1 & \
    gst-launch-1.0 videotestsrc pattern=pinwheel ! "video/x-raw,format=NV61,width=1920,height=1080,framerate=30/1" ! v4l2sink device=/dev/video/mvdu0
fi

if test $CMD -eq 8 ; then
    echo "Start combined NNA / MVDU test"
    config_mvdu
    gst-launch-1.0 videotestsrc ! "video/x-raw,format=RGB,width=1920,height=1080" ! queue leaky=downstream ! v4l2convert disable-passthrough=true capture-io-mode=4 output-io-mode=4 ! v4l2sink device=/dev/video/mvdu0
fi

if test $CMD -eq 9 ; then
    echo "Start camera 0 at isp0 + camera 3 at isp1 (pip)"
    config_mvdu
    gst-launch-1.0 -e v4l2src device=/dev/video/isp0-hv ! "video/x-raw,format=NV61,width=1920,height=1080" ! v4l2sink device=/dev/video/mvdu0 \
                      v4l2src device=/dev/video/isp1-hv ! "video/x-raw,format=NV61,width=1920,height=1080" ! v4l2sink device=/dev/video/mvdu1
fi

if test $CMD -eq 10 ; then
    echo "Start camera 0 at isp0 + nna + camera 3 at isp1 (pip)"
    config_mvdu
    gst-launch-1.0 -e v4l2src device=/dev/video/isp0-hv ! "video/x-raw,format=RGB,width=1920,height=1080,framerate=15/1" ! queue leaky=downstream ! v4l2convert disable-passthrough=true capture-io-mode=4 output-io-mode=4 ! v4l2sink device=/dev/video/mvdu0 \
                      v4l2src device=/dev/video/isp1-hv ! "video/x-raw,format=NV61,width=1920,height=1080" ! queue leaky=downstream ! v4l2sink device=/dev/video/mvdu1
fi

if test $CMD -eq 11 ; then
    echo "Start MVDU only test (VID + OSD)"
    config_mvdu
    gst-launch-1.0 videotestsrc pattern=blue ! "video/x-raw,format=BGRA,width=1920,height=1080,framerate=30/1" ! v4l2sink device=/dev/video/mvdu2 \
                   videotestsrc pattern=pinwheel ! "video/x-raw,format=NV61,width=1920,height=1080,framerate=30/1" ! v4l2sink device=/dev/video/mvdu0
fi

if test $CMD -eq 12 ; then
    echo "Start MVDU only test (OSD)"
    config_mvdu
    gst-launch-1.0 videotestsrc pattern=pinwheel ! "video/x-raw,format=BGRA,width=1920,height=1080,framerate=10/1" ! v4l2sink device=/dev/video/mvdu2
fi

if test $CMD -eq 13; then
	  echo "Start NNA overlay pipeline: isp_pre->isp_main->nna->mvdu (overlay) over testpattern"
	  config_mvdu
	  gst-launch-1.0 v4l2src device=/dev/video/isp0-hv ! "video/x-raw,format=RGB,width=1920,height=1080,framerate=15/1" ! queue leaky=downstream ! v4l2convert disable-passthrough=true capture-io-mode=4 output-io-mode=4 ! "video/x-raw,format=BGRA,width=1920,height=1080" ! v4l2sink device=/dev/video/mvdu2 \
		               videotestsrc pattern=pinwheel ! "video/x-raw,format=NV61,width=1920,height=1080,framerate=15/1" ! v4l2sink device=/dev/video/mvdu0
fi

