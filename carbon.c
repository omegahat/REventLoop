#ifndef NO_CARBON

/*
 Can we do input sources with 
    CFReadStreamSetClient
    CFReadStreamScheduleWithRunLoop
  See http://developer.apple.com/techpubs/macosx/ReleaseNotes/CoreFoundation.html
*/
#include "Reventloop.h"

/* From RCarbon.h */
#define __MRC__

#ifdef __MRC__  /* Apple' C/C++ Compiler */
  #include <Carbon.h>
#else /* CodeWarrior */
  #include <MacHeadersCarbon.h>
#endif /* __MRC__ */

void R_runCarbonEventLoop(void);
void R_carbonReceiveNextEvent();


SEXP R_carbonInstallTimer(int interval, R_TimerFunc f, void *userData);
int R_carbonRemoveTimer(SEXP rep)

int R_carbonRemoveIdle(SEXP rep);
SEXP R_carbonInstallIdle(R_IdleFunc f, void *userData);

R_EventLoop R_CarbonEventLoop = {

  "Carbon",
  NULL,
  NULL,
  R_runCarbonEventLoop,
  R_receiveNextEvent, /* Non blocking iteration! */
  QuitEventLoop,

  NULL, /* addInput */
  R_carbonInstallIdle, /* addIdle */
  R_carbonInstallTimer, /* addTimer */

  NULL, /* removeInput */
  R_carbonRemoveIdle, /* removeIdle */
  R_carbonRemoveTimer, /* removeTimer */

  NULL, /* instance-specific data */

  NULL, /* next */
  NULL  /* prev */
};

void
R_runCarbonEventLoop(void)
{
#if 1
  RunApplicationEventLoop();
#else
  RunCurrentEventLoop(kEventDurationForever);
#endif
}


/** 
  Process one event if it is there and return immediately. 
*/
int
R_carbonReceiveNextEvent()
{
  EventRef ev;
  OSStatus status;
  status = ReciveNextEvent(0, NULL, kEventDurationNoWait, true, &ev);
  EventTargetRef target = GetEventDispatcherTarget();

  if (err == noErr) {
      (void) SendEventToEventTarget(ev, target);
      ReleaseEvent(ev);
      return(1);
  }

  return(0);
}


void 
R_carbonTimerAction (EventLoopTimerRef  theTimer, void* userData)
{

}


SEXP
R_carbonInstallTimer(int interval, R_TimerFunc f, void *userData)
{
  EventLoopRef loop = GetCurrentEventLoop();
  EventTimerInterval delay, time;
  EventLoopTimerUPP proc;
  EventLoopTimerRef out;
  OSStatus status; 

  delay = 0;
  time = interval * kEventDurationSecond; /* Is this milli sceonds */

  status = InstallEventLoopTimer(loop, delay, time, NewEventLoopTimerUPP(proc), userData, &out);
  if(status != noErr) {
   PROBLEM "Can't register timer task"
   ERROR;
  }

  return(createRCarbonTimer(out));
}

int 
R_carbonRemoveTimer(SEXP rep)
{
  EventLoopTimerRef ref;
  OSStatus status;

  ref = getCarbonTimer(rep);
  status = RemoveEventLoopTimer(ref);

  return( status == noErr ? 1 : 0);
}


/*
  void IdleTimerAction (EventLoopTimerRef inTimer, void *inUserData, EventIdleAction inAction );
*/
SEXP
R_carbonInstallIdle(R_IdleFunc f, void *userData)
{
  EventLoopRef loop = GetCurrentEventLoop();
  EventTimerInterval delay, time;
  EventLoopTimerUPP proc;
  EventLoopTimerRef out;
  OSSStatus status;

  delay = 0;
  time = 0;

/* 
  This is not quite what we want. It will not detect other events such as input onf
  streams, etc. 
  And it is only available on OS X 10.2.
*/
  status = InstallEventLoopIdleTimer(loop, delay, time, proc, userData, &out);
  if(status != noErr) {
   PROBLEM "Can't register idle task"
   ERROR;
  }

  return(createRCarbonTimer(out));
}

int
R_carbonRemoveIdle(SEXP rep)
{
  EventLoopTimerRef ref;
  OSStatus status;

  ref = getCarbonTimer(rep);
  status = RemoveEventLoopTimer(ref);

  return( status == noErr ? 1 : 0); 
}


#endif /* NO_CARBON */
