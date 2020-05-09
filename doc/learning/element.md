## 提示

### 无浮层，体制内

Alert - el-alert

有特殊的字体、颜色、和背景，可带 icon、和辅助性文字介绍，用于页面中展示重要的提示信息。


### 无浮层，体制外，轻量级就近信息提示
Tooltip    - el-tooltip
Popover    - el-popover
Popconfirm - el-popconfirm


### 浮层，体制外，不黑屏、轻量级提醒

Notification - this.$notify
悬浮出现在页面角落，显示全局的通知提醒消息。

Message - this.$message
常用于主动操作后的反馈提示。与 Notification 的区别是后者更多用于系统级通知的被动提醒。


### 浮层，体制外，黑屏、带操作交互
MessageBox
模拟系统的消息提示框而实现的一套模态对话框组件，用于消息提示、确认消息和提交内容。
当用户进行操作时会被触发，该对话框中断用户操作，直到用户确认知晓后才可关闭。

从场景上说，MessageBox 的作用是美化系统自带的 alert、confirm 和 prompt，因此适合展
示较为简单的内容。如果需要弹出较为复杂的内容，请使用 Dialog。

* alert 消息提示 - this.$alert

* confirm 确认消息 - this.$confirm

* promit 提交内容 - this.$prompt

* msgbox 自定义内容 - this.$msgbox

Dialog - el-dialog
在保留当前页面状态的情况下，告知用户并承载相关操作。
Dialog 弹出一个对话框，适合需要定制性更大的场景。
