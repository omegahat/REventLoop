#include "Reventloop.h"

#include "Rdefines.h"

SEXP R_getEventLoop();
SEXP getREventLoopObject();

enum {ELOOP_NAME_SLOT, ELOOP_ADDRESS_SLOT, ELOOP_SLOT_COUNT};

SEXP
R_setEventLoop(SEXP sym)
{
  SEXP ans;

  ans = R_getEventLoop();

  if(length(sym)) {
     SEXP s;
     void *tmp = NULL;
     if(TYPEOF(sym) == LISTSXP) {
       s = VECTOR_ELT(sym, ELOOP_ADDRESS_SLOT);
     } else {
       s = sym;
     }
     if(length(s) && TYPEOF(s) == EXTPTRSXP) /*  && R_ExternalPtrTag(s) == Rf_install("REventLoop") */
       tmp = (R_EventLoop*) R_ExternalPtrAddr(s);

     R_eloop = tmp;
  }

  return(ans);
}

SEXP
R_getEventLoop()
{
  SEXP ans, tmp;
  /* Will move to S4 classes very soon when we have a NEW() C symbol for these. */
  if(!R_eloop) {
     return(R_NilValue);
  }

#if S4_CLASSES

  PROTECT(ans = getREventLoopObject());
   /* Have to create _and populate_ the character vector before calling SET_SLOT().
      Otherwise, if we create it in the call to SET_SLOT() and populate later, it comes
      out empty.*/
  PROTECT(tmp = NEW_CHARACTER(1));
  if(R_eloop->name)
      SET_STRING_ELT(tmp, 0, mkChar(R_eloop->name));
  SET_SLOT(ans, Rf_install("name"), tmp);

  SET_SLOT(ans, Rf_install("address"), R_MakeExternalPtr(R_eloop, Rf_install("REventLoop"), R_NilValue));
  UNPROTECT(2);
#else

  PROTECT(ans = NEW_LIST(2));  
  SET_VECTOR_ELT(ans, ELOOP_NAME_SLOT, tmp = NEW_CHARACTER(1));
  if(R_eloop->name)
      SET_STRING_ELT(tmp, 0, mkChar(R_eloop->name));

  SET_VECTOR_ELT(ans, ELOOP_ADDRESS_SLOT, R_MakeExternalPtr(R_eloop, Rf_install("REventLoop"), R_NilValue));

  PROTECT(tmp = NEW_CHARACTER(2));
  SET_VECTOR_ELT(tmp, ELOOP_NAME_SLOT, mkChar("name"));
  SET_VECTOR_ELT(tmp, ELOOP_ADDRESS_SLOT, mkChar("address"));
  SET_NAMES(ans, tmp);

  PROTECT(tmp = NEW_CHARACTER(1));
  SET_VECTOR_ELT(tmp, 0, mkChar("REventLoop"));
  SET_CLASS(ans, tmp);

  UNPROTECT(3);
#endif

  return(ans);
}

SEXP
R_getEventLoopName()
{
 SEXP ans;
 PROTECT(ans = allocVector(STRSXP, 1));
 if(R_eloop)
    SET_STRING_ELT(ans, 0, mkChar(R_eloop->name ? R_eloop->name : "?"));
 UNPROTECT(1); 
 return(ans);
}


SEXP
getREventLoopObject()
{
  SEXP e, ans, tmp;
  int errorOccurred;

  PROTECT(e = allocVector(LANGSXP, 2));
  SETCAR(e, Rf_install("new"));
  SETCAR(CDR(e), (tmp = NEW_CHARACTER(1)));
  SET_STRING_ELT(tmp, 0, mkChar("REventLoop"));

  ans = R_tryEval(e, R_GlobalEnv, &errorOccurred);

  UNPROTECT(1);

  return(ans);
}
