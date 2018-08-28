# python语法


# 数据结构

    x = True
    y = False

    s = 'xyzopq' + str(123)
    la = ['aaa', 'bbb', 'ccc']
    lb = (1, 2, 3)
    o = {'one': 1, 'two': 2, 'three': 3, 'four': 4}
    rdb = {
        'dbs': {
            'main': 0,
            'log': 9,
            'middle': 8
        },
        'sss': {
            'nearby': '2f45e2fe148771177e8460211f905f4abc74b264',
        }
    }

    rdb['dbs']['main']

    # 空字符串判断
    if not s:

    # 空list判断
    if not la:
    # or
    if len(lb) == 0:

    # 空对象判断
    if not o:
    # or
    if len(o) == 0

    # 对象属性判断
    if 'three' in o:

# 语法

    # 遍历list
    for x in la:
        print(x)
    # or
    >>> S = [1,30,20,30,2]
    >>> for index, elem in enumerate(S):
            print(index, elem)

    (0, 1)
    (1, 30)
    (2, 20)
    (3, 30)
    (4, 2)

    # 遍历对象
    for x in o:
        if x == 'four':
            o[x]                # 4

    # if
    if (x && (y || !z)):
        print('xxx')
    elif m:
        print('xxx')
    else:

# 函数和模块

#rdb.py
def getshop(shopid):
    "返回商铺信息"
    return r.hgetall('adola_tag_' + str(shopid))

#main.py
import rdb as RDB
shopinfo = RDB.getshop(shopid)


# 调试

    > help(la)

    > "First argument: {0}, second one: {1}".format(47,11)

    'First argument: 47, second one: 11'

    > q = 459
    > p = 0.098
    > print(q, p, p * q, sep=" :-) ")

    459 :-) 0.098 :-) 44.982

    print '处理场所' + spotid

    type("Hello world")      # <type 'str'>
    type(10.4*3)             # <type 'float'>
    type(103)                # <type 'int'>
    type(type)               # <type 'type'>
    type(True)               # <type 'bool'>

# 方法


# 全局
