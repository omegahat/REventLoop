\name{addIdle}
\alias{addIdle}
\alias{addTimer}
\title{Register background or timed actions}
\description{
  These provide an event-loop-independent manner to specify
  actions that should happen in the background when there are
  no events to process, or at a particular time in the future.
  The action can be given as a function, expression, call
  or any language object which can be evaluated.
  The mechanism evaluates the action at the appropriate
  time and can reschedule it if the result of the action
  is the logical value \code{TRUE}.
  This is a simple way to run actions indefinitely
  or to have them terminate when a given condition
  arises.
}
\usage{
addIdle(f, data = NULL)
addTimer(interval, f, data = NULL)
}
\arguments{
  \item{f}{the action to be invoked.
    This can be a function, a call or an expression.
    If it is a function, one can use the \code{data} argument to arrange
    to have a value passed to the function to parameterize its actions.}
  \item{data}{an arbitrary S value that is passed to the action
    if it is a function. This can be used to parameterize a function
    so that it can be used for different actions that behave differently
    depending on this \code{data} argument.
    The value is passed as the first and only argument to the action function.}
  \item{interval}{number of milli-seconds to wait
    before the action is invoked. While every effort to
    execute the action at that time is made, it is not possible
    to guarantee it will happen at exactly that time; just no sooner.
    Other conditions on the machine (e.g. load) may influence when the
    action is actually triggered.}
}
\details{
  This uses the C-level API for the abstract event loop
  to register the S action with that event processor.
}
\value{
  An identifier for the registered action.
  This can be used to remove/unregister that action.
  (Will be implemented in the future!)
}
\references{\url{http://www.omegahat.org/REventLoop}}
\author{Duncan Temple Lang <duncan@research.bell-labs.com>}


\seealso{
  \code{\link{runEventLoop}}
  \code{\link{eventLoop}}
}

\examples{
\dontrun{
   # Print "Still here" every 5 seconds!
  addTimer(5000, quote({cat("Still here\n"); TRUE}))


   # Same thing but using a general function rather than
   # an expression.
  timerMessage <- function(msg) {
    cat(msg, "\n")
    TRUE
  }
  addTimer(5000, timerMessage, "Still here")


    # Keep a count of the number of times
    # action was called after each 0.8 seconds
    # Uses a global variable: BAD!
  assign("ctr", 0, env = globalenv())
  addTimer(800, function() {
                   myCtr <<- myCtr + 1
                   cat("myCtr", myCtr, "\n")
                   TRUE  # run again
                })

    # Same thing with a closure so no global variables
    # GOOD 
  counter <- function(ctr = 0) {
     function() {
            ctr <<- ctr + 1
             cat("Counter", ctr, "\n")
            TRUE  # run again
     }
  }
  addTimer(800, counter)

   # Suppose nextPage() shows the next frame or display
   # in an animation of plots. This walks through those
   # pages, waiting 1 second between pages.
  addTimer(1000, nextPage)

  # In the background, download a collection of URLs
  # This is probably too long an action to be suitable for
  # an idle action.

  idleDownload  <- function(urls, dir = "/tmp") {

      # need to use a try() to catch any errors to correctly move past a
      # problematic URL.
    status <- download.file(urls[1], destfile = paste(dir, basename(urls),
                             sep=.Platform$file.sep))

    if(status) {
       # Remove the recently downloaded url.
      urls <- urls[-1]
      cat("Fetched", urls, "\n")
    }

    return(length(urls))
  }


  addIdle(idleDownload(c("http://www.omegahat.org/REventLoop/index.html",
                         "http://cran.r-project.org/index.html")))

}
}
\keyword{interface}

