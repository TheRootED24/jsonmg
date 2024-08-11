# jsonmg
A basic JSON parsing and serialization library. Provides high level Lua binding to the Mongoose C Neworking library to allow reading and writing JSON data with minimal overhead

# Build
```
git clone https://github.com/TheRootED24/jsonmg.git
cd jsonmg
cmake .
make
```
# Usage
```

lua
Lua 5.1.5  Copyright (C) 1994-2012 Lua.org, PUC-Rio (double int32)

> jsonmg = require "jsonmg"
> array = jsonmg:stringify({"I", "am", "a", "JSON", "string", "array"})
> print(array)

["I","am","a","JSON","string","array"]

> lua_array = jsonmg:parse(array)
> for i = 1, #lua_array do
>> print(lua_array[i])
>> end

I
am
a
JSON
string
array

> object = jsonmg:stringify({I = "am", VERY = "simple", JSON = "object"})
> print(object)

{"I":"am","JSON":"object","VERY":"simple"}  **Note that in Lua, the order returned for key value tables is not respected, where as an numeric indexed (array) table does

> object = jsonmg:stringify({I = "am", A = { "JSON",  {object = {true}}}})
> print(object)

{"I":"am","A":["JSON",{"object":[true]}]}

> nestedArray = jsonmg:stringify({"string", 100, true, {1,2,3}, 4,5,6})
> print(nestedArray)

["string",100,true,[1,2,3],4,5,6]

> nestedObj = jsonmg:stringify({I ="am", AN = "object", A = { NESTED = { object = true}}}) ** again oreder was not preserved, but the output is valid JSON just the same
> print(nestedObj)

{"I":"am","A":{"NESTED":{"object":true}},"AN":"object"}

