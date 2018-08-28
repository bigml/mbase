// mjs语法


// 数据结构

    let x = true;
    let y = false;
    // TODO null undefined NaN

    let s = 'xyzopq' + 123 + 'zzz';
    let a = ['aaa', 'bbb', 'ccc'];
    let o = {foo: 1, bar: 2, baz: 3, four: 4, five: 5};
    let rdb = {
        dbs: {
            main: 0,
            log: 9,
            middle: 8,
        },
        sss: {
            exist: "LEAF_HASH",
            nearby: '2f45e2fe148771177e8460211f905f4abc74b264',
        },
    },

    rdb['dbs']['main']  or rdb.dbs.main

    // 空字符串判断
    if (s === '') 或 if (s.length <= 0)

    // 空数组判断
    if (a.length <= 0)

    // 空对象判断
    if (JSON.stringify(o) === '{}')

    // 对象属性判断
    if (!o['unexist'])

    // TODO undefined 处理
    if (unexist)
        // balabala
    // 会抛出 undefined 异常

// 语法

    // 遍历数组
    for (let i = 0; i < a.length; i++) {
        let node = a[i]
    }

    // 遍历对象
    for (let k in o) {
        if (k === 'four') {
            let v = o[k];  // 4
        }
    }

    // if
    if (x && (y || !z)) {
    } else if (m) {
    } else {
    }

// 函数

    let httpPost = function(url) {
    };

    let o = {
        version: 1.1,
        httpPost: function(url) {
        },
    }

// 调试

    console.log(JSON.stringify(o))

    typeof 1 === 'number' &&
    typeof('foo') === 'string' && typeof '' === 'string' &&
    typeof(1 === 2) === 'boolean' && typeof(true) === 'boolean' && typeof false === 'boolean' &&
    typeof[] === 'array' &&
    typeof({}) === 'object' &&
    typeof print === 'foreign_ptr' &&
    typeof(function() {}) === 'function' && typeof null === 'null' && typeof undefined === 'undefined';

// 方法

    a.length;                                      // 3
    a.join(':sep:');                               // 'aaa:sep:bbb:sep:ccc'
    a.push('ddd');                                 // ['aaa', 'bbb', 'ccc', 'ddd']
    a.splice(1, 2, 'mmm', 'nnn', 'ooo', 'ppp');    // "['aaa', 'mmm', 'nnn', 'ooo', 'ppp', 'ddd']"

    s.length;
    s.slice();
    s.at();
    s.indexOf();

// 全局

    ffi()
    load()
    global,print,ffi_cb_free,mkstr,getMJS,die,gc,chr

    JSON.stringify(a) === "['aaa', 'bbb', 'ccc']"
    JSON.parse()

    Object.create()

    NaN
    isNaN()

    TIME.systime()
    OS.usleep()
    SYS.parseInt()
    SYS.objDelete()
    SYS.regMatch()
