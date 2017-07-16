## 事件处理

$.on();
$.off();
$.onece();
$.trigger();


### event.stopImmediatePropagation() 方法阻止剩下的事件处理程序被执行。

### event.stopPropagation() 方法阻止事件冒泡到父元素，阻止任何父事件处理程序被执行。

### event.preventDefault() 方法阻止元素发生默认的行为（常用于阻止链接跳转）。

e.preventDefault() will prevent the default event from occuring,
e.stopPropagation() will prevent the event from bubbling up and
return false will do both.

Note that this behaviour differs from normal (non-jQuery) event handlers, in which,
notably, return false does not stop the event from bubbling up.
