#include <stdio.h>
#include <string.h>

#include <readline/readline.h>

#include <Rinternals.h>
#include <Rdefines.h>
#include <IOStuff.h>
#include <R_ext/eventloop.h>


extern DL_FUNC ptr_R_ReadConsole;

extern int UsingReadline;


/* Must be the same as that in $R_HOME/src/main/main.c */
typedef struct {
  int            status;
  int            prompt_type;
  int            browselevel;
  unsigned char  buf[1025];
  unsigned char *bufp;
} R_ReplState;

/*XX */
extern char *R_PromptString(int browselevel, int type);
extern int Rf_ReplIteration(SEXP rho, int savestack, int browselevel, R_ReplState *state);


/* Readline variables */

typedef struct _R_ReadlineData R_ReadlineData;

/* This had better have the same structure as the one in R
   for the common fields.  Then we add extra fields to our
   definition.
*/
struct _R_ReadlineData {
 int readline_gotaline;
 int readline_addtohistory;
 int readline_len;
 int readline_eof;
 unsigned char *readline_buf;
 R_ReadlineData *prev;

 unsigned char ReadlineBuffer[10000];

 Rboolean quitting;
 R_ReplState state;
 Rboolean quitOnLine;
 
 SEXP InputId;

 unsigned char ReadlinePrompt[20];
};

R_ReadlineData *rl_top = NULL;

SEXP addReadlineHandler(void *);
int removeReadlineHandler(void *data, SEXP id);

void readlineHandler(char *line);
void resetReadline(R_ReadlineData *);
static int local_Rstd_ReadConsole(char *prompt, unsigned char *buf, int len,  int addtohistory);

#include "Reventloop.h"

extern R_EventLoop R_GtkEventLoop;

R_EventLoop *R_eloop = &R_GtkEventLoop;

int copyREventLoop(R_EventLoop *target, InputHandler *handlers);

/*
 See R_ToplevelExec() for ideas for handling the contexts.
 */
void
R_mainLoop(long *copy)
{
  RCNTXT newContext;
  RCNTXT * volatile saveToplevelContext;
  volatile SEXP topExp;
  int  (*oldReadConsole)(char *, unsigned char *, int, int);

  R_ReadlineData rl_data = {0, 0, 0, 0, NULL, NULL, "", FALSE, {0, 1, 0, "", NULL}, FALSE, 0, "+--+"};

  char **argv;
  int argc = 1;

  oldReadConsole = NULL;

  if(!R_eloop) {
    PROBLEM "No event loop set"
    ERROR;
  }
    

  if(R_eloop->init) {
     argv = malloc(sizeof(char *) * argc);
     argv[0] = strdup("R");

     R_eloop->init(&argc, &argv);
  }

  oldReadConsole = ptr_R_ReadConsole;
  ptr_R_ReadConsole = (DL_FUNC) local_Rstd_ReadConsole;


  if(*copy) {
        /* Get any thing that is already on the R event loop. */
     copyREventLoop(R_eloop, R_InputHandlers->next);
  }



  PROTECT(topExp = R_CurrentExpr);
  saveToplevelContext = R_ToplevelContext;

  R_IoBufferInit(&R_ConsoleIob); /* This may be very important! */

        /* Ensure errors, etc. bring us back to here. */ 
  begincontext(&newContext, CTXT_TOPLEVEL, R_NilValue, R_GlobalEnv, R_NilValue, R_NilValue);

  if(SETJMP(newContext.cjmpbuf) == 0) {
     R_GlobalContext = R_ToplevelContext = &newContext;
  }

     /* Really have to make certain that this is done only 
        if it is not already on the event source list. */
  rl_data.InputId = addReadlineHandler(&rl_data);

    /* Now handle the events indefinitely until gtk_main_quit() is called. */
  R_eloop->main();
  removeReadlineHandler(&rl_data, rl_data.InputId);

  if(R_eloop->init) 
    free(argv);

  endcontext(&newContext); 
  R_ToplevelContext = saveToplevelContext;
  R_CurrentExpr = topExp;
  ptr_R_ReadConsole = oldReadConsole;

  UNPROTECT(1); 
}


void
R_runEventLoop(long *copy)
{
  char **argv = NULL;
  int argc = 1;

  if(!R_eloop)
    return;

  if(R_eloop->init) {
     argv = (char **) malloc(sizeof(char *) * argc);
     argv[0] = strdup("R");

     R_eloop->init(&argc, &argv);
  }

  if(*copy) {
        /* Get any thing that is already on the R event loop. */
     copyREventLoop(R_eloop, R_InputHandlers->next);
  }

  if(R_eloop)
     R_eloop->main();

  if(R_eloop->init)
     free(argv);
}


/**********************************************************************/
/* Readline related material */

/* Interactively called to quit from this current event loop without emitting
   an extra prompt. Asynchronous/non-interactive calls can invoke gtk_main_quit() 
   directly.  This is only for interactive commands. Calling this never hurts!*/
void
R_event_loop_quit()
{
   R_ReadlineData *d = rl_top;
   if(!R_eloop) {
     PROBLEM "No active event loop"
     ERROR;
   }

   if(d)
     d->quitting = TRUE;

   R_eloop->quit();
}


void
ReadlineHandler(void *data, int fd, int mask)
{
   R_ReadlineData *d = (R_ReadlineData *) data;

   d->prev = rl_top;
   rl_top = d;

   rl_callback_read_char();

   rl_top = d->prev;
   d->prev = NULL;

   if(d->readline_gotaline) {
      if(d->state.prompt_type == 2)
        memcpy(d->state.buf + strlen(d->state.buf)-1, d->readline_buf, 1024 - strlen(d->state.buf));
      else
        memcpy(d->state.buf, d->readline_buf, 1024);
      d->state.bufp = d->state.buf;
/*      d->state.prompt_type = 1; */
      R_IoBufferReadReset(&R_ConsoleIob);
      R_IoBufferWriteReset(&R_ConsoleIob);
      removeReadlineHandler(d, d->InputId);

/* d->state.prompt_type != 2 && */
      while(*d->state.bufp) {
        int status;
	status = Rf_ReplIteration(R_GlobalEnv, R_PPStackTop, 0, &d->state);
/*
	if(status == 2) {
   	   d->state.bufp = d->state.buf + strlen(d->state.buf);
	}
*/
      }

       /* if we aren't in the middle of a gtk_main_quit
          invoked manually, then emit a prompt. Otherwise,
          don't bother. */
      if(!d->quitting) {
        d->InputId = addReadlineHandler(d);
      }
   }
}

int
removeReadlineHandler(void *data, SEXP id)
{
   int status;
   rl_callback_handler_remove();
   status = R_eloop->removeInput(id);
   R_ReleaseObject(id);

   return(status);
}

void
readlineHandler(char *line)
{
  int l;
  R_ReadlineData *d = rl_top;

    rl_callback_handler_remove();

    if ((d->readline_eof = !line)) /* Yes, I don't mean ==...*/
	return;
    if (line[0]) {
	l = (((d->readline_len-2) > strlen(line))?
	     strlen(line): (d->readline_len-2));
	strncpy((char *)d->readline_buf, line, l);
	d->readline_buf[l] = '\n';
	d->readline_buf[l+1] = '\0';
    }
    else {
	d->readline_buf[0] = '\n';
	d->readline_buf[1] = '\0';
    }
    d->readline_gotaline = 1;
}

void
resetReadline(R_ReadlineData *d)
{
   char *prompt = R_PromptString(d->state.browselevel, d->state.prompt_type);
   if(d->state.prompt_type != 2 || 1) {
      d->readline_gotaline = 0;
      d->readline_buf = d->ReadlineBuffer;
      d->readline_len = 0;
      d->readline_eof = 0;
   }
   rl_callback_handler_install(prompt, readlineHandler);
}

SEXP
addReadlineHandler(void *data)
{
 SEXP id;
 resetReadline((R_ReadlineData *) data);
 id = R_eloop->addInput(fileno(stdin), ReadlineHandler, data);
 R_PreserveObject(id);
 return(id);
}



/****************************************************************/

/* Timeout routine used in an S sleep command simply to break out of the current event loop. */
static int
Rf_MainQuit(void *unused)
{
  R_eloop->quit();
  return(0); /* Don't reset. */
}

SEXP
R_sleep(SEXP interval)
{
  R_eloop->addTimer(asInteger(interval), Rf_MainQuit, NULL);
  R_eloop->main();
  return(R_NilValue);  
}

/****************************************************************/

/* scan() handler */

void
scanReadlineHandler(void *data, int fd, int mask)
{
   R_ReadlineData *d = (R_ReadlineData *) data;

   d->prev = rl_top;
   rl_top = d;

   rl_callback_read_char();

   rl_top = d->prev;
   d->prev = NULL;  
   if(d->readline_gotaline) {
      R_eloop->quit();
      d->quitting = TRUE;
      rl_callback_handler_remove();
   }
}

static int 
local_Rstd_ReadConsole(char *prompt, unsigned char *buf, int len,
 	         int addtohistory)
{
    if(!R_Interactive) {
	int ll;
	if (!R_Slave)
	    fputs(prompt, stdout);
	if (fgets((char *)buf, len, stdin) == NULL)
	    return 0;
	ll = strlen((char *)buf);
	/* remove CR in CRLF ending */
	if (ll >= 2 && buf[ll - 1] == '\n' && buf[ll - 2] == '\r') {
	    buf[ll - 2] = '\n';
	    buf[--ll] = '\0';    
	}
/* according to system.txt, should be terminated in \n, so check this
   at eof */
	if (feof(stdin) && (ll == 0 || buf[ll - 1] != '\n') && ll < len) {
	    buf[ll++] = '\n'; buf[ll] = '\0';
	}
	if (!R_Slave)
	    fputs((char *)buf, stdout);
	return 1;
    }
    else {
        int status = 0;
        R_ReadlineData scan_rl_data;
	SEXP id = NULL;
/* #ifdef HAVE_LIBREADLINE */
	if (UsingReadline) {
	    scan_rl_data.readline_gotaline = 0;
	    scan_rl_data.readline_buf = buf; /* scan_rl_data.ReadlineBuffer */
/* 	    scan_rl_data.readline_addtohistory = addtohistory; */
	    scan_rl_data.readline_len = len;
	    scan_rl_data.readline_eof = 0;
	    scan_rl_data.quitOnLine = TRUE;
	    scan_rl_data.quitting = FALSE;
	    rl_callback_handler_install(prompt, readlineHandler);
            id = R_eloop->addInput(fileno(stdin), scanReadlineHandler, &scan_rl_data);
	    PROTECT(id);
	}
	else
/* #endif  HAVE_LIBREADLINE */
	{
	    fputs(prompt, stdout);
	    fflush(stdout);
	}

        R_eloop->main();

	if(id) {
   	  removeReadlineHandler(&scan_rl_data, id);
	  UNPROTECT(1);
	}
	if (scan_rl_data.readline_eof)
		status = 0;
	if (scan_rl_data.readline_gotaline)
		status = 1;
	return(status);
       }
}



/* 
  Generic facility for adding one event loop
  to another by registering it as an idle
  routine.
  This can be done more generically by
  grabbing the non-blocking iteration for that one.
*/
SEXP
R_localAddIdle(R_IdleFunc f, void *userData)
{
  SEXP ans;

    /* May want to do this as a timer. Contention issues. */
  ans = R_eloop->addIdle(f, userData);

  return(ans);
}

typedef struct {
  InputHandlerProc fun;
  void *data;
  int fd;
} R_InputHandlerLoopData; 

void
R_genericEventLoopInputDispatch(void *ptr, int fd, int mask)
{
  R_InputHandlerLoopData *data = (R_InputHandlerLoopData *)ptr;
  data->fun(data->data);
}


/*
 */
int
copyREventLoop(R_EventLoop *target, InputHandler *handlers)
{
  int n = 0;  
  while(handlers) {
     R_InputHandlerLoopData *tmp;
     tmp = (R_InputHandlerLoopData *) R_alloc(1, sizeof(R_InputHandlerLoopData));
     tmp->fun = handlers->handler;
     tmp->data = handlers->userData;
     tmp->fd = handlers->fileDescriptor;

     R_eloop->addInput(handlers->fileDescriptor, R_genericEventLoopInputDispatch, tmp);
     handlers = handlers->next;
     n++;
  }
  return(n);
}


/***************************************************/

int
R_timerCallback(void *data)
{
    R_CallbackData *cbdata = (R_CallbackData *) data;
    int val = FALSE;
    SEXP e, sval;
    int errorOccurred;

    if(isFunction(cbdata->function)) {
      PROTECT(e = allocVector(LANGSXP, 1 + (cbdata->useData == TRUE ? 1 : 0)));

      SETCAR(e, cbdata->function);
      if(cbdata->useData) {
	SETCAR(CDR(e), cbdata->data);
      }
    } else
      PROTECT(e = cbdata->function); /* Unprotect is not necessary here. Just for symmetry*/
  
    sval = R_tryEval(e, R_GlobalEnv, &errorOccurred);

    if(!errorOccurred) {
 	if(IS_LOGICAL(sval) == FALSE) {
            if(isFunction(cbdata->function)) {
                 /* Does this create a last.warning in the user's session because it is 
                    executed as a top-level task? So wiill the user see anything.
                    However, she can call warnings(). */
    	       PROBLEM  "This %s handler didn't return a logical value. Removing it.\n",
                         cbdata->type == RGTK_TIMER ? "timer" : "idle"
               WARN;
            }
	    val = FALSE;
	} else
	    val = LOGICAL(sval)[0];
    }
    UNPROTECT(1);

    return(val);
}


static R_CallbackData *
createCallbackData(SEXP sfunc, SEXP data, SEXP useData)
{
  R_CallbackData *cbdata;
  cbdata = (R_CallbackData*) malloc(sizeof(R_CallbackData));

  R_PreserveObject(sfunc);
  cbdata->function = sfunc;
  cbdata->type = RGTK_TIMER;
  if(LOGICAL(useData)[0]) {
      R_PreserveObject(data);
      cbdata->data = data;
      cbdata->useData = TRUE;
  } else {
      cbdata->useData = FALSE;
      cbdata->data = NULL;
  }

  return(cbdata);
}

SEXP
R_AddTimeout(SEXP sinterval, SEXP sfunc, SEXP data, SEXP useData)
{
  SEXP ans;
  R_CallbackData *cbdata;
  cbdata = createCallbackData(sfunc, data, useData);
 
  ans = R_eloop->addTimer(INTEGER(sinterval)[0], R_timerCallback, cbdata);

  return(ans);
}

SEXP
R_AddIdle(SEXP sfunc, SEXP data, SEXP useData)
{
  SEXP ans;
  R_CallbackData *cbdata;
  cbdata = createCallbackData(sfunc, data, useData);
  ans = R_eloop->addIdle(R_timerCallback, cbdata);

  return(ans);
}
