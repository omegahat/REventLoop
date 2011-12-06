Sys.msleep <-
  # milli-seconds
function(time)
{
    # The user level mechanism.
  if(FALSE) {
     gtkAddTimeout(as.integer(time), function(...){quitEventLoop(); return(FALSE)})
     runEventLoop(repl = FALSE)
   } else 
     .Call("R_sleep", as.integer(time))
 
  cat("Finishing sleep\n")

  invisible(TRUE)
}  
