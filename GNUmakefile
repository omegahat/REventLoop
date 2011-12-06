ifdef ADMIN
  include Install/GNUmakefile.admin
endif

REventLoop.so:

TCL_LIB=-ltk8.4 
# -ltcl8.3 

SRC=Reventloop.c tcltk.c gtk.c Rroutines.c
OBJS=$(SRC:%.c=%.o)

GTK_CONFIG=gtk-config

DEFINES=-DS4_CLASSES

CFLAGS=-g -Wall -pedantic $(shell $(GTK_CONFIG) --cflags) -I$(R_HOME)/src/include $(DEFINES)

READLINE_LIB=-lreadline -lncurses


$(OBJS): Reventloop.h

REventLoop.so: $(OBJS)
	(PKG_LIBS="$(shell $(GTK_CONFIG) --libs) $(READLINE_LIB) $(TCL_LIB)" ; export PKG_LIBS ; R CMD SHLIB -o $@ $^)


gtkReader: gtkTestReadline.o Reventloop.c
	$(CC) -o $@ $^ $(shell $(GTK_CONFIG) --libs) $(READLINE_LIB)

clean:
	-rm $(OBJS)

distclean:
	-rm REventLoop.so
