#ifndef NO_TCL_TK

#include <tcl.h>
#include <tk.h>

#include <Rdefines.h>
#include <IOStuff.h>
#include <R_ext/eventloop.h>
#include "Reventloop.h"


void tcl_main();
int tcl_non_blocking_iteration();
void tcl_quit();
SEXP tclAddInput(int fd, void (*f)(void *, int, int), void *userData);
SEXP tclAddTimer(int interval, R_TimerFunc, void *userData);
void tclInit(int *argc, char ***argv);
SEXP tclAddIdle(int (*fun)(void *), void *userData);
int  R_tclRemoveTimer(SEXP id);
int  R_tclRemoveIdle(SEXP id);
int R_tclRemoveInput(SEXP id);/* No op currently */

R_EventLoop R_TclEventLoop = {
                     "TclTk",
	             tclInit,
		     NULL,
	             &tcl_main,
		     NULL, /* gtk_main_iteration_do */
		     &tcl_quit,

                     &tclAddInput, 
                     &tclAddIdle,  
                     &tclAddTimer, 

		     R_tclRemoveInput,
		     R_tclRemoveIdle,
		     R_tclRemoveTimer
                    };


/* Tcl/Tk event loop on its own. */

void
R_tclMainLoop()
{
  R_EventLoop *oldLoop = R_eloop; 

  R_eloop = &R_TclEventLoop;
  R_mainLoop();
  R_eloop = oldLoop;
}

Rboolean tclEventLoopQuit = FALSE;

void
tcl_main()
{
   while(Tcl_DoOneEvent(TCL_ALL_EVENTS)) {
     if(tclEventLoopQuit) {
        tclEventLoopQuit = FALSE;
	break;
     }
   }
}

int
tcl_non_blocking_iteration()
{
  return(Tcl_DoOneEvent(TCL_ALL_EVENTS));
}

void
tcl_quit() 
{
  tclEventLoopQuit = TRUE;
}

SEXP
tclAddInput(int fd, void (*f)(void *, int, int), void *userData)
{
   /* Perhaps put the info together into a list of 
      external pointers and allow the user to return these
      to suspend/remove the. For deleting the handler, we only
      need the file descriptor apparently.
      Suspending is done with a value of 0 for the mask.
      Do we need the proc (i.e. f) and the user data also to match.
    */
  SEXP ans;
  Tcl_CreateFileHandler(fd, TCL_READABLE, f, userData);
  ans = allocVector(INTSXP, 1);
  INTEGER(ans)[0] = fd;
  return(ans);
}


/* This structure and R_tclTimerDispatch() provide a 
   Gtk-like interface for timeout actions in that 
   it allows them to return a value indicating whether
   the should be rescheduled or not.
   There are issues about garbage collection which need
   to be smoothed out across different event loop types, 
   specifically Tcl/Tk's odd mechanism for removing
   timeouts.
   */
typedef struct {
  R_TimerFunc fun;  
  void *data;
  int interval;
} R_TclTimerData;


void
R_tclTimerDispatch(void *data)
{
  R_TclTimerData *d = (R_TclTimerData *)data;
  int again;
  again = d->fun(d->data);
  if(again) {
    (void) Tcl_CreateTimerHandler(d->interval, R_tclTimerDispatch, d);
  } else
    free(d);
}

SEXP
tclAddTimer(int interval, R_TimerFunc fun, void *userData)
{
  Tcl_TimerToken tok;
  SEXP ans;
  R_TclTimerData *rd;

  rd = (R_TclTimerData *) malloc(sizeof(R_TclTimerData));
  rd->fun = fun; rd->interval = interval; rd->data = userData;
  tok = Tcl_CreateTimerHandler(interval, R_tclTimerDispatch, rd);

/* Tcl_TimerTokens are normally structs. But they seem to have integer values! */
#if 1
  ans = R_MakeExternalPtr(tok, Rf_install("Tcl_TimerToken"), R_NilValue);
#else
  ans = NEW_NUMERIC(1);
  NUMERIC(ans)[0] = (double) tok;
#endif
  return(ans);
}

int
R_tclRemoveInput(SEXP id)
{
  return(0);
}

int
R_tclRemoveTimer(SEXP id)
{
   Tcl_TimerToken tok;
   tok = (Tcl_TimerToken) R_ExternalPtrAddr(id);
   Tcl_DeleteTimerHandler(tok);
   return(TRUE);
}

typedef struct {
  R_IdleFunc fun;
  void *data;
} R_TclIdleData; 

void
R_tclIdleDispatch(void *data)
{
 int again;
 R_TclIdleData *d = (R_TclIdleData *)data;
 again = d->fun(d->data);
 if(again) {
   Tcl_DoWhenIdle(R_tclTimerDispatch, data);
 } else
   free(data);
}

SEXP
tclAddIdle(int (*fun)(void *), void *userData)
{
  SEXP ans;
  R_TclIdleData *rd;

  rd = (R_TclIdleData *) malloc(sizeof(R_TclIdleData));
  rd->fun = fun; rd->data = userData;

  Tcl_DoWhenIdle(R_tclIdleDispatch, rd);

  ans = R_MakeExternalPtr(rd, Rf_install("R_TclIdleData"), R_NilValue);

  return(ans);
}

int
R_tclRemoveIdle(SEXP id)
{
 R_TclIdleData *data;
 data = (R_TclIdleData *) R_ExternalPtrAddr(id);
 if(!data)
    return(FALSE);

 Tcl_CancelIdleCall(R_tclIdleDispatch, data);

 return(TRUE);
}



/* Taken primarily from tcltk package's source code. */
static Tcl_Interp *RTcl_interp = NULL;
void
tclInit(int *argc, char ***argv)
{
    int code;
    if(!RTcl_interp) 
      RTcl_interp = Tcl_CreateInterp();

    code = Tcl_Init(RTcl_interp); /* Undocumented... If omitted, you
				    get the windows but no event
				    handling. */
    if (code != TCL_OK)
	error(RTcl_interp->result);

    code = Tk_Init(RTcl_interp);  /* Load Tk into interpreter */
    if (code != TCL_OK)
	error(RTcl_interp->result);

    Tcl_StaticPackage(RTcl_interp, "Tk", Tk_Init, Tk_SafeInit);

    code = Tcl_Eval(RTcl_interp, "wm withdraw .");  /* Hide window */
    if (code != TCL_OK)
	error(RTcl_interp->result);

#if TEST_TK
#endif
}

void
R_testTk(char **fileName)
{
    int code;
    if(RTcl_interp == NULL) {
       int argc = 1;
       char *argv[] = {"R"};
       tclInit(&argc, &argv);
    }
/*	error("You must have created the Tcl interpreter before calling this test routine"); */

    code = Tcl_EvalFile(RTcl_interp, fileName[0]);
    if (code != TCL_OK)
      error("%s\n", RTcl_interp->result);
}


/***************************************************/
/* Tcltk as an idle event on the gtk event loop. */

int
gtkTclTkIdle(void *data)
{
#if 0
    fprintf(stderr, "Doing tcl events idly\n");fflush(stderr);
#endif
    Tcl_DoOneEvent(TCL_DONT_WAIT | TCL_ALL_EVENTS);
    return(1);
}



/*
 Registers a tcl/tk event ``loop'' as an idle task on the Gtk event loop.

 This may be better as a timeout.
*/
SEXP
R_addTclTkToGtk()
{
  return(R_localAddIdle(gtkTclTkIdle, NULL));
}

#endif
