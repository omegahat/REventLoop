require(RGtk)
w <- gtkWindow(show = FALSE)
b <- gtkButton("Hit me")
b$AddCallback("clicked", function(btn) { x <- 1; y <-2 ; browser() ; TRUE})
w$Add(b)
w$Show()

