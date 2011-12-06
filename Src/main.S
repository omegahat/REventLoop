runEventLoop <-
function(repl = TRUE, loop = NULL, copy = TRUE)
{
  if(!is.null(loop)) {
     # set the event loop.
    old <- eventLoop(loop)
    on.exit(eventLoop(old))
  }

  if(is.character(repl)) {
   oldPrompt <- options()$prompt
   options(prompt = repl)
   on.exit(options(prompt = oldPrompt))
   repl <- TRUE
  }

  .C(ifelse(repl, "R_mainLoop", "R_runEventLoop"), as.logical(copy))
}


setGeneric("eventLoop", function(sym) standardGeneric("eventLoop"))

setMethod("eventLoop", "missing", function(sym)    .Call("R_getEventLoop") )

setMethod("eventLoop", "character", function(sym)
                                       eventLoop(getNativeSymbolInfo(sym)))

setMethod("eventLoop", "NativeSymbolInfo", function(sym)
                                              eventLoop(REventLoop(sym$name, sym$address)))

setMethod("eventLoop", "REventLoop", function(sym) .Call("R_setEventLoop", sym@address))
   


