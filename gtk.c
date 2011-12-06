#ifndef NO_GTK

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "Reventloop.h"

/*SEXP gtkAddInput(int fd,  void (*handler)(void *, int fd, int), void *userData); */
SEXP gtkAddInput(int fd,  void (*handler)(void *, int, GdkInputCondition), void *userData);
SEXP R_gtkAddIdle(R_IdleFunc f, void *userData);
SEXP R_gtkAddTimeout(int interval, R_IdleFunc f, void *userData);
int R_gtkRemoveIdle(SEXP id);
int R_gtkRemoveTimeout(SEXP id);
int R_gtkRemoveInput(SEXP id);
/*
 Get warnings for gtk_idle_add and gtk_timeout_add
 but they are "essentialy" correct.
 But we need to generalize them to return a SEXP
 since that is the only "common" type we can handle
 extensibly.
*/
R_EventLoop R_GtkEventLoop = {
                     "GTK",
	             &gtk_init,
		     NULL,
	             &gtk_main,
		     &gtk_main_iteration_do,
		     &gtk_main_quit,
                     &gtkAddInput,   
                     &R_gtkAddIdle,  
                     &R_gtkAddTimeout, 

		     &R_gtkRemoveInput,
		     &R_gtkRemoveIdle, 
		     &R_gtkRemoveTimeout 
                    };


SEXP R_addGtkToTclTk();

SEXP
R_gtkAddIdle(R_IdleFunc f, void *userData)
{
 SEXP ans;
 ans = allocVector(REALSXP, 1);
 REAL(ans)[0] = gtk_idle_add(f, userData);
 return(ans);
}


int
R_gtkRemoveIdle(SEXP id)
{
  gtk_idle_remove(REAL(id)[0]);
  return(TRUE);
}

SEXP
R_gtkAddTimeout(int interval, R_IdleFunc f, void *userData)
{
 SEXP ans;
 ans = allocVector(REALSXP, 1);
 REAL(ans)[0] = gtk_timeout_add(interval, f, userData);
 return(ans);
}

int
R_gtkRemoveTimeout(SEXP id)
{
  gtk_timeout_remove(REAL(id)[0]);
  return(TRUE);
}



/* 
  This processes a single Gtk event and returns. It ensures that
  it is restored.
  See gtkTclTkIdle for equivalent routine in opposite direction. */
gint
tclTkGtkIdle(gpointer data)
{
    gtk_main_iteration_do(FALSE);
/*    R_addGtkToTclTk(); */
    return(1);
}


SEXP
R_addGtkToTclTk()
{
  return(R_localAddIdle(tclTkGtkIdle, NULL));
}


/*
 We get a warning since the signature for Gdk is 
 void (*GdkInputFunction) (gpointer, gint, GdkInputCondition condition)
 and GdkInputCondition is an enum with 3 states.
*/
SEXP
gtkAddInput(int fd,  void (*handler)(void *, int, GdkInputCondition), void *userData)
{
 SEXP ans;
 ans = allocVector(REALSXP, 1);
 REAL(ans)[0] = gtk_input_add_full(fd, GDK_INPUT_READ, handler, NULL, userData, NULL);
 return(ans);
}

int
R_gtkRemoveInput(SEXP id)
{
  gtk_input_remove(REAL(id)[0]);
  return(TRUE);
}

#ifdef UNUSED
void
readlineMarshal(GtkObject *obj, gpointer data, guint nargs, GtkArg *args)
{
  fprintf(stderr,"[Marshal] number of arguments: %d\n", nargs);
}
#endif



#endif /* NO_GTK */
