\name{runEventLoop}
\alias{runEventLoop}
\alias{quitEventLoop}
\title{Run/Terminate external event loop}
\description{
  \code{runEventLoop} runs the current event loop data structure
  registered with the \code{REventLoop} package, handling all the events
  it knows. When its termination or quit method is
  invoked, control is returned to the original event loop.
  This allows for nested event loops.

  The event loop can be run in either of two ways: i) as an interactive
  replacement for the standard R event loop, providing a prompt
  for user input; and ii) as a non-interactive event loop that
  ensures that GUIs, etc. remain active while not allowing
  user-input from the console.
  The latter approach is used for implementing dialogs,
  or simply when there is a GUI from which we get user input
  and no console input is desired.

  \code{quitEventLoop} terminates or exits
  the currently active event loop, returning control
  to the previous or outer event loop.
}
\usage{
runEventLoop(repl = TRUE, loop = NULL, copy = TRUE)
quitEventLoop()
}
\arguments{
  \item{repl}{a logical value indicating to run the REPL version of the
    event loop or simply the basic event-processing loop.
    The REPL handles emitting prompts and uses readline.}
  \item{loop}{
    an optional object of class \code{REventLoop}.
    If this is specified and identifies a C symbol
    (in the address slot), this address is used
    as the event loop to run, rather than the current one
    on the top of the event loop stack.
  }
  \item{copy}{a logical value indicating whether to
    copy the currently registered event sources and handlers
    from the standard R event loop.
    This is typically done just once when one has started
    an X11 graphics device before runnin an event loop from this
    package. If this is \code{TRUE} under these circumstances,
    the X11 event handler for the X11 device windows are
    registered with the new event loop and will still be responsive.
    Otherwise, no events wil processed for them under the new event loop.
    Similarly, if you start an event loop from this package and
    subsequently open an X11 device, it will not receive any events and hence
    won't resize or redraw.

    In the future, we will integrate this general event loop mechanism
    into the R source code and make the different devices use it
    when registering event handlers, processing locator requests, etc.
  }
}
\details{
 See the Docs/ directory of the REventLoop package.
}
\value{
 Empty list (\code{list()}).
}
\references{\url{http://www.omegahat.org/REventLoop}}
\author{Duncan Temple Lang <duncan@research.bell-labs.com>}


\seealso{
  \code{\link{quitEventLoop}}
}

\examples{
\dontrun{
  runEventLoop()
  library(RGtk)
  source(system.file("examples", "testgtk.S", package = "RGtk")
}
}
\keyword{interface}
