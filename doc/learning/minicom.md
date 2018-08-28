## minicom 使用

1. $ sudo minicom -s 进入 Serial port setup 设置串口

  * A Serial Device 指定好串口名

  * E Bps 9600~115200自适应适用 115200 8N1 

  * F Hardware Flow Control Yes

  * G Software Flow Control No

2. Save setup as 为当前的设置取个名字， e.g. usb0

3. Exit From minicom

4. $ sudo minicom -o usb0

   Ctrl - a  o 进入 Serial port setup

   选择 F Hardware Flow Control 为 No

   Exit 

   此时便能输入文字和查看接收到的数据

5. Ctrl - a q 退出 minicom


注： 在 minicom 中 使用 Ctrl - a e 可以控制输入回显
