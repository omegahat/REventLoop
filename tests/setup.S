library(tcltk)
library(RGtk)


# Use the GTK device rather than the X11 one.
# This means events will be handled for this also
# without any extra work to include the X11 events on
# the Gtk event loop.
library(gtkDevice)
options(device="gtk")


# Load the replacement event loop code.
dyn.load("gtkReader.so")

# Change the prompt and start our own event loop...
options(prompt=">>")
invisible(.C("R_mainLoop"))




