#!/usr/bin/env python
#
# Plays a test screen to screen.
#
# Equivalent to:
#    gst-launch-1.0 videotestsrc ! videoconvert ! autovideosink
#

import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib
import os

Gst.init(None)
mainloop = GLib.MainLoop()

pipeline =  Gst.Pipeline.new("pipe")

src = Gst.ElementFactory.make("videotestsrc")
convert = Gst.ElementFactory.make("v4l2convert")
sink = Gst.ElementFactory.make("autovideosink")

pipeline.add(src)
pipeline.add(convert)
pipeline.add(sink)

src.link(convert)
convert.link(sink)

src.set_property("num-buffers", 100)
convert.set_property("capture-io-mode", 4)
convert.set_property("output-io-mode", 4)
convert.set_property("disable-passthrough", True)

pipeline.set_state(Gst.State.PLAYING)
mainloop.run()

