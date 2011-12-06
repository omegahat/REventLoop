\name{REventLoop}
\alias{REventLoop}
\title{Create Event Loop Object}
\description{
  This is a generic function that
  is used to create an S object that represents
  an R event loop.
  Such objects are of class \code{REventLoop}
  and have just a name and a reference to a
  C-level data structure that provides the
  methods that implement the particular event loop.

}
\usage{
REventLoop(name, address)
}
\arguments{
  \item{name}{the user-leve name to use for the event loop.
  This does not have to be in any way related to the name of the C
  variable used to to define the event loop.
  It is just a name that can be used to remind the user of the type of the event
  loop.
  If \code{address} is not specified, this name is used
  as the name of the C symbol.
  }
  \item{address}{
    the address of the C symbol that implements the event loop
    functionality.
    This can be provided in several forms: the name of the C variable
    which is resolved using \code{\link[base]{getNativeSymbolInfo}};
    an object of class \code{NativeSymbolInfo} previously resolved
    using \code{\link[base]{getNativeSymbolInfo}};
    or an object of class \code{NativeSymbol}.
  }
}
\value{
  An obect of class \code{REventLoop}.
  This has two slots:
  \item{name}{A user-understandable name for the event loop}
  \item{address}{the internal memory address of the C variable that
    implements the event loop}
}

\references{\url{http://www.omegahat.org/REventLoop}}
\author{Duncan Temple Lang <duncan@research.bell-labs.com>}
\seealso{
  \code{\link{eventLoop}}
  \code{\link{runEventLoop}}  
}

\examples{
\dontrun{
 eventLoop(REventLoop("R_TclEventLoop"))

 REventLoop("R_GtkEventLoop")

 REventLoop("GTK", getNativeSymbolInfo("R_GtkEventLoop", PACKAGE="REventLoop"))
}
}
\keyword{interface}

