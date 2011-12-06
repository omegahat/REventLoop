require(methods)


setOldClass("NativeSymbol")
setOldClass("NativeSymbolInfo")

setClass("REventLoop",
          representation(name="character",
                         address="externalptr"))


setGeneric("REventLoop", function(name, address) standardGeneric("REventLoop"))
setMethod("REventLoop", signature("character", "character"), function(name, address) REventLoop(name = name, address = getNativeSymbolInfo(address)))
setMethod("REventLoop", signature("character", "missing"), function(name, address) REventLoop(name = name, address = getNativeSymbolInfo(name)))
setMethod("REventLoop", signature("character", "NativeSymbol"), function(name, address) REventLoop(name, unclass(address)))
setMethod("REventLoop", signature("character", "NativeSymbolInfo"), function(name, address) REventLoop(name, address$address))


setMethod("REventLoop", signature("character", "externalptr"), function(name, address) new("REventLoop", name = name, address = address))

