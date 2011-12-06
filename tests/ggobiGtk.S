
source("tk.S")

if(require(Rggobi))
  ggobi("/home/duncan/Projects/ggobi/ggobi-cvs/data/flea.xml")


f <- system.file("examples", "testgtk.S", package="RGtk")
if(f != "")
 source(f)


