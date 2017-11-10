## lua

x = {foo: "aaa", bar: "bbb"}

> x = {foo = function(a,b) return a end, bar = function(a,b) return b end, }
> return x.foo(3,4)
3
> return x.bar(3,4)
4
> return x:foo(3,4)
table: 0x10a120
> return x:bar(3,4)
3

The colon is for implementing methods that pass self as the first parameter. So x:bar(3,4)should be the same
as x.bar(x,3,4).




## js

let x = {foo: "aaa", bar: "bbb"}
