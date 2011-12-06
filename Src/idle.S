addTimer <-
function(interval, f, data = NULL)
{
 useData <- !missing(data)

 if(!is.function(f) && !is.language(f))
   stop("Timeout handlers must be function, expression or call objects")
 
 .Call("R_AddTimeout", as.integer(interval),  f, data, useData)  
}

addIdle <-
function(f, data = NULL)
{
 useData <- !missing(data)

 if(!is.function(f) && !is.language(f))
   stop("Idle handlers must be function, expression or call objects") 
 
 .Call("R_AddIdle",  f, data, useData)  
}  
