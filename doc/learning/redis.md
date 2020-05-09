### 调试

e.g. $redis-cli --eval exist.lua adola_foods adola_food_ name , 测试家的牛肉

      $redis-cli script load "$(cat exist.lua)"
      >EVALSHA d01a0f3046b623ba57f354be41baeb15b805c2f5 3 adola_foods adola_food_ name 测试家的牛肉
      >eval "return {KEYS[1],KEYS[2],ARGV[1],ARGV[2]}" 2 key1 key2 first second

      $redis-cli --ldb --eval ./exist.lua adola_foods adola_food_ name , 测试家的牛肉



### 删除
$ redis-cli keys user* | xargs redis-cli del


### 查看日志
$ redis-cli monitor


### 显示中文
ml@VM-0-4-debian:~$ redis-cli --raw
127.0.0.1:6379> select 7
OK
127.0.0.1:6379[7]> hgetall book_9787539135502
title
克里克塔


### 数据结构
出了常规的 list, set, sorted set, hash 之外，有几种数据结构值得注意

* geo

* bitmap

* streams

* skip list

* [RedisJSON](https://github.com/RedisJSON/RedisJSON)

* [RedisGraph](https://github.com/RedisGraph/RedisGraph)
