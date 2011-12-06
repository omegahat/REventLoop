\name{eventLoop}
\alias{eventLoop}
\title{Register an (internal) Event Loop implementation}
\description{
  This sets an internal C variable that holds the
  default event loop that can be run using \code{\link{runEventLoop}}.
  This doesn't actually run the event loop; it just sets which one
  will be used when it is needed.
  The registered event loop should be a C \code{R_EventLoop}
  data structure with methods for all of the different
  facilities that provides.
}
\usage{
eventLoop(sym)
}
\arguments{
  \item{sym}{the name or address of a C symbol identifying
    the event loop data structure that contains the different
    methods}
}
\value{
  An object of class \code{REventLoop}
  representing the previously registered event loop.

}
\references{\url{http://www.omegahat.org/REventLoop}}
\author{Duncan Temple Lang <duncan@research.bell-labs.com>}

\seealso{
\code{\link{runEventLoop}}
}

\examples{
\dontrun{
 eventLoop("R_TclEventLoop")

 old <- eventLoop("R_TclEventLoop")
  # run some Tk code
 quitEventLoop()
 eventLoop(old)
}
}
\keyword{interface}

