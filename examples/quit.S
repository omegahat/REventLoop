library(RGtk)

w <- gtkWindow(show = FALSE)
b <- gtkButton("Quit event loop")
b$AddCallback("clicked", function(...) .C("gtk_main_quit"))
w$Add(b)
w$Show()

