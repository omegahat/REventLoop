# For the moment, these can be left to the different
# packages that provide an interface to the toolkits,
# e.g. RGtk and tcltk 
#
# In the future, we will want to use the abstraction
# which will allow us to write code independent of the
# toolkit/event loop currently active and also
# allow us to switch between them and copy event sources.

addInput <-
function(con, fun, data)
{
  
}

removeInput <-
function(id)
{

}  

addIdle <-
function(f, data)
{

}

removeIdle <-
function(id)
{

}

addTimer <-
function(interval, f, data)
{

}

removeTimer <-
function(id)
{

}

