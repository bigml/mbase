### notice

There is no difference between:

> LPUSH myset 123
> LPUSH myset "123"

### Redis to Lua conversion table.

* Redis integer reply -> Lua number
* Redis bulk reply -> Lua string
* Redis multi bulk reply -> Lua table (may have other Redis data types nested)
* Redis status reply -> Lua table with a single ok field containing the status
* Redis error reply -> Lua table with a single err field containing the error
* Redis Nil bulk reply and Nil multi bulk reply -> Lua false boolean type


### Lua to Redis conversion table.

* Lua number -> Redis integer reply (the number is converted into an integer)
* Lua string -> Redis bulk reply
* Lua table (array) -> Redis multi bulk reply (truncated to the first nil inside the Lua array if any)
* Lua table with a single ok field -> Redis status reply
* Lua table with a single err field -> Redis error reply
* Lua boolean false -> Redis Nil bulk reply.

There is an additional Lua-to-Redis conversion rule that has no corresponding Redis to Lua conversion rule:

* Lua boolean true -> Redis integer reply with value of 1.
* Also there are two important rules to note:

Lua has a single numerical type, Lua numbers. There is no distinction between integers and floats. So we
always convert Lua numbers into integer replies, removing the decimal part of the number if any. If you want
to return a float from Lua you should return it as a string, exactly like Redis itself does (see for
instance the ZSCORE command).

There is no simple way to have nils inside Lua arrays, this is a result of Lua table semantics, so when
Redis converts a Lua array into Redis protocol the conversion is stopped if a nil is encountered.

Here are a few conversion examples:

> eval "return 10" 0
(integer) 10

> eval "return {1,2,{3,'Hello World!'}}" 0
1) (integer) 1
2) (integer) 2
3) 1) (integer) 3
   2) "Hello World!"

> eval "return redis.call('get','foo')" 0
"bar"
The last example shows how it is possible to receive the exact return value of redis.call() or redis.pcall()
from Lua that would be returned if the command was called directly.

In the following example we can see how floats and arrays with nils are handled:

> eval "return {1,2,3.3333,'foo',nil,'bar'}" 0
1) (integer) 1
2) (integer) 2
3) (integer) 3
4) "foo"
As you can see 3.333 is converted into 3, and the bar string is never returned as there is a nil before.
