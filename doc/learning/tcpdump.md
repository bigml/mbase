#### 状态 ####

* FIN

    表示发送端曾经到达数据末路，也即便说双边的数据递交告终，未曾数据能够递交了，发送FIN符号位的TCP数据包后，连接将被断开。这个符号的数据包也经常被用于举行端口扫描。
    当一个FIN符号的TCP数据包发送到一台计算机的特定端口，万一这台计算机响应了这个数据，并且反馈归来一个RST符号的TCP包，就阐明这台计算机上未曾敞开这个端口，然而这台计算机是存在的；
    万一这台计算机未曾反馈归来任何数据包，这就阐明，这台被扫描的计算机存在这个端口。

* URG

    此符号表示TCP包的紧迫指针域（后面即刻即将说到）管用，用来保证TCP连接不被间断，并且监督其中层装备要尽快处理这些数据；

* ACK

    此符号表示应答域管用，即便说前面所说的TCP应答号将会包括在TCP数据包中；有两个取值：0和1，为1的时候表示应答域管用，反之为0；

* PSH

    这个符号位表示Push垄断。所谓Push垄断即便指在数据包到达接收端尔后，即刻递交给利用过程，而不是在缓冲区中排队；

* RST

    这个符号表示连接复位哀求。用来复位那些发生讹谬的连接，也被用往返绝讹谬和违法的数据包；

* SYN

    表示同步序号，用来发生连接。SYN符号位和ACK符号位搭配利用，当连接哀求的时候，SYN=1，ACK=0；连接被相应的时候，SYN=1，ACK=1；
    这个符号的数据包经常被用来举行端口扫描。扫描者发送一个只有SYN的数据包，万一对手主机响应了一个数据包归来，就阐明这台主机存在这个端口；
    然而由于这种扫描措施只是举行TCP三次握手的第顺次握手，因而这种扫描的获胜表示被扫描的机器不很平安，一台平安的主机将会迫使要求一个连接严厉的举行TCP的三次握手；


#### 基础 ####

    tcpdump: listening on fxp0
    11:58:47.873028 202.102.245.40.netbios-ns > 202.102.245.127.netbios-ns: udp 50
    11:58:47.974331 0:10:7b:8:3a:56 > 1:80:c2:0:0:0 802.1d ui/C len=43
                           0000 0000 0080 0000 1007 cf08 0900 0000
                           0e80 0000 902b 4695 0980 8701 0014 0002
                           000f 0000 902b 4695 0008 00
    11:58:48.373134 0:0:e8:5b:6d:85 > Broadcast sap e0 ui/C len=97
                           ffff 0060 0004 ffff ffff ffff ffff ffff
                           0452 ffff ffff 0000 e85b 6d85 4008 0002
                           0640 4d41 5354 4552 5f57 4542 0000 0000
                           0000 00

tcpdump -xxx expression

普通情况下，直接启动tcpdump将监视第一个网络界面上所有流过的数据包。

tcpdump对截获的数据并没有进行彻底解码，数据包内的大部分内容是使用十六进制的形式直接打印输出的。
显然这不利于分析网络故障，通常的解决办法是先使用带-w参数的tcpdump 截获数据并保存到文件中，
然后再使用其他程序进行解码分析。当然也应该定义过滤规则，以避免捕获的数据包填满整个硬盘。

* 使用-i参数指定tcpdump监听的网络界面，这在计算机具有多个网络界面时非常有用，
* 使用-c参数指定要监听的数据包数量，
* 使用-w参数指定将监听到的数据包写入文件中保存
* 使用-X参数以十六进制和文本格式打印消息体（除链路层），与-s 0 结合使用可打印全长度消息。
    When parsing and printing, in addition to printing the headers of each packet, print the data of each packet (minus its link  level  header)  in  hex  and
    ASCII.  This is very handy for analysing new protocols.

* 抓包并实时查看 websocket 二进制数据
  sudo tcpdump -l -w - dst host s50.37wandtsh5.5jli.com | tcpflow -C -D -r - | while read line; do  echo $line | sed 's/0000: //' | sed 's/ \.\..*//' | sed 's/ //g' | xargs ./unws; done

  sudo tcpdump -s 0 -l -w - dst host pdk.pkgame.net and port 8500 | tcpflow -C -D -r - | awk -F'[0-9]{4}: ' '{print $2; fflush()}' | awk -F[..] '{print $1; fflush()}' | sed -u 's/ //g' | ./callunws.sh

  -l line buffer
  -w - 输出到stdou
  -C console print without packet source and destination detail
  -D console out should be in hex
  -r - 从stdin 读入
  fflush() 关闭 awk buffer功能
  -u unbuffer


expression为以下几种情况：

第一种是关于类型的关键字，主要包括host，net，port, 例如 host 210.27.48.2，
指明 210.27.48.2是一台主机，net 202.0.0.0 指明 202.0.0.0是一个网络地址，port 23 指明端口号是23。
如果没有指定类型，缺省的类型是host.

第二种是确定传输方向的关键字，主要包括src , dst ,dst or src, dst and src,
这些关键字指明了传输的方向。举例说明，src 210.27.48.2 ,指明ip包中源地址是210.27.48.2 ,
dst net 202.0.0.0 指明目的网络地址是202.0.0.0 。如果没有指明方向关键字，则缺省是src or dst关键字。

第三种是协议的关键字，主要包括fddi,ip,arp,rarp,tcp,udp等类型。
Fddi指明是在FDDI(分布式光纤数据接口网络)上的特定 的网络协议，实际上它是"ether"的别名，
fddi和ether具有类似的源地址和目的地址，所以可以将fddi协议包当作ether的包进行处理和 分析。
其他的几个关键字就是指明了监听的包的协议内容。如果没有指定任何协议，则tcpdump将会监听所有协议的信息包。

除了这三种类型的关键字之外，其他重要的关键字如下：gateway, broadcast,less,greater,
还有三种逻辑运算，取非运算是 'not ' '! ', 与运算是'and','&&';或运算 是'or' ,'││'；
这些关键字可以组合起来构成强大的组合条件来满足人们的需要，下面举几个例子来说明。


#### 例子 ####

* tcpdump host 210.27.48.1

  截获所有210.27.48.1 的主机收到的和发出的所有的数据包：

* tcpdump host 210.27.48.1 and \ (210.27.48.2 or 210.27.48.3 \)

  截获主机210.27.48.1 和主机210.27.48.2 或210.27.48.3的通信，
使用命令：（在命令行中适用　括号时，一定要

* tcpdump ip host 210.27.48.1 and ! 210.27.48.2

  获取主机210.27.48.1除了和主机210.27.48.2之外所有主机通信的ip包，使用命令：

* tcpdump tcp port 23 and host 210.27.48.1

  获取主机210.27.48.1接收或发出的telnet包，使用如下命令：

* tcpdump udp port 123

  对本机的udp 123 端口进行监视 123 为ntp的服务端口

* tcpdump -i eth0 src host hostname

  系统将只对名为hostname的主机的通信数据包进行监视。
主机名可以是本地主机，也可以是网络上的任何一台计算机。下面的命令可以读取主机hostname发送的
所有数据：

* tcpdump -i eth0 dst host hostname

  下面的命令可以监视所有送到主机hostname的数据包：

* tcpdump -i eth0 gateway Gatewayname

  我们还可以监视通过指定网关的数据包：

* tcpdump -i eth0 host hostname and port 80

  监视编址到指定端口的TCP或UDP数据包，那么执行以下命令：

* tcpdump ip host 210.27.48.1 and ! 210.27.48.2

  如果想要获取主机210.27.48.1除了和主机210.27.48.2之外所有主机通信的ip包，使用命令：

* ! tcp contains "Heartbeat" and (ip.dst==69.90.18.212 || ip.src==69.90.18.212) and tcp.flags.push == 1
