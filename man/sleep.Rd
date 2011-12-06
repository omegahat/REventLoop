\name{Sys.msleep}
\alias{Sys.msleep}
\title{Blocking sleep with active event handlers}
\description{

  This allows one to temporarily pause before
  the evaluation of the next R expression
  for a "fixed" period of time.  This version of sleep differs
  from \code{\link{Sys.sleep}}  in two ways:
   \item{a}{it has a finer resolution for the time interval,
     using milliseconds rather than simply seconds.}
   \item{b}{while "sleeping", events are still
     processed.  This means that windows, idle tasks,
     etc. are still responsive.}
}
\usage{
Sys.msleep(time)
}
\arguments{
  \item{time}{the number of milliseconds to sleep}
}
\details{
  This uses the general event loop mechanism.
  It starts by registering an timer callback.
  The callback is setup to terminate the active
  event loop. Then we immediately start a secondary
  instance of the event loop. This process all the events
  while waiting for the timer callback to terminate it.
  When this happens, the event loop terminates and returns
  control to the caller and hence \code{Sys.msleep} returns.
}
\value{
  \code{TRUE}
}
\references{}
\author{Duncan Temple Lang <duncan@research.bell-labs.com>}

\seealso{
 \code{\link{runEventLoop}}
}

\examples{
\dontrun{
 runEventLoop()
 if(require(RGtk)) {
   source(system.file("examples", "testgtk.S", package = "RGtk"))
   h$progressBar()
 }
 Sys.msleep(1000)
}
}
\keyword{interface}

