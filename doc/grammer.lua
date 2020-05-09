-- redis lua script 语法


-- 数据结构

    local x = true
    local y = false
    local z = nil               -- 一个无效直，类似 js 中的undefined，在条件表达式中相当于false

    local s = 'xyzopq' .. 123 .. 'zzz'
    local ta = {'aaa', 'bbb', 'ccc'}
    local tb = {foo = 1, bar = 2, baz = 3, four = 4, five = 5}
    local rdb = {
        dbs = {
            main = 0,
            log = 9,
            middle = 8,
        },
        sss = {
            exist = "LEAF_HASH",
            nearby = '2f45e2fe148771177e8460211f905f4abc74b264',
        },
    }

    -- 数组索引从1开始
    tb[1] -- 'aaa'

    rdb['dbs']['main']  or rdb.dbs.main

    -- 空字符串判断
    if (s == nil or s == '') then end

    -- 空数组判断
    if next(ta) == nil then
    end

    -- 空对象判断
    if next(tb) == nil then
    end

    -- 对象属性判断
    if not tb['unexist'] then end

-- 语法

    -- 遍历数组
    for i, v in ipairs(ta) do
       if i ~= 3 then
          -- v 'ccc'
       end
    end

    -- 遍历对象
    for k, v in pairs(tb) do
        if k == 'four' then
           -- tb[k] == v == 4
        end
    end

    -- if
    if (x and (y or not z)) then
        -- balabala
    elseif m then
        -- balabala
    else
        -- balabala
    end

-- 函数

    local httpPost = function(url)
       -- balabala
    end

-- 调试

    print("xxxx" .. pos .. string.sub(msg, pos, pos + 2))

    type("Hello world")      --> string
    type(10.4*3)             --> number
    type(print)              --> function
    type(type)               --> function
    type(true)               --> boolean
    type(nil)                --> nil
    type(type(X))            --> string

-- 方法
    string gsub fromhex char format len
    os getenv
    io lines
    tonumber


-- 全局

   redis.replicate_commands()
   redis.call('TIME')
   redis.call('HINCRBY', 'cargo_' .. cargoid, 'read_num', 1);
   redis.call('ZADD', 'read_history_' .. uuid, utc, cargoid)

   cjson.encode(tb)
