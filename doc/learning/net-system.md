#### 原理 ####

TCP和UDP在发送网络数据包的时候，是先发送到socket缓冲区，再到系统缓冲区，然后再由系统通过网络设备发送出去；
接收的过程则相反。send或receive并不是等到数据包发送出去才返回，而是在等待socket缓冲区。
在发送的时候，如果socket缓冲区满了，send函数就会调用sbwait去等待缓冲区空闲；
在接收的时候，如果socket缓冲区是空的，receive函数就会调用sbwait去等到缓冲区产生数据。
因此当进程挂在sbwait的时候，如果是发送导致的挂起，就要检查下socket缓冲区的设置是否太小，满
足不了数据传输的要求；
更大的可能是网络拥塞，因为TCP在发送数据后需要等待对端的ACK确认才会继续发送后续的数据，
如果因为网络拥塞对端收不到数据或者socket本身收不到对端的ACK，就会停止发送数据，导致socket缓冲区被写满；
对端的缓冲区太小也会导致这样的问题，因为发送的速度超过接收的速度，缓冲区在一段时间后也会被写满。



#### 方法 ####

* setsockopt
    改变socket缓冲区大小，等待的超时时间，心跳包检测时间等。

* sysctl 命令行
    对服务器进行全局临时设置，

    sysctl -a | grep net 可以用来查看网络相关的配置信息

    sysctl -q net.ipv4.tcp_window_scaling 查看某个具体配置的值

    sysctl -w net.ipv4.tcp_window_scaling=1 临时修改某个配置值

* sysctl -p /etc/sysctl.conf
    对服务器进行全局永久设置。

* netstat -s
    用来查看当前网络的统计信息，-st查看tcp, -su查看udp信息，可以用来检查丢包问题发生在那一
    层，配合 iptable -L 查看相关信息。


#### 调优参数 ####

##### net.core #####

* net.core.somaxconn
* net.core.rmem_max
    最大的TCP数据接收缓冲。

* net.core.wmem_max
    最大的TCP数据发送缓冲。

* net.core.rmem_default
    默认的接收窗口大小。

* net.core.wmem_default
    默认的发送窗口大小。



##### net.ipv4 #####

* net.ipv4.tcp_syn_retries
    默认值是5
    对于一个新建连接，内核要发送多少个 SYN 连接请求才决定放弃。不应该大于255，默认值是5，对应于180秒左右时间。
    (对于大负载而物理通信良好的网络而言,这个值偏高,可修改为2.这个值仅仅是针对对外的连接,对进来的连接,是由tcp_retries1 决定的)

* net.ipv4.tcp_synack_retries ：INTEGER
    默认值是5
    对于远端的连接请求SYN，内核会发送SYN ＋ ACK数据报，以确认收到上一个 SYN连接请求包。这
    是所谓的三次握手( threeway handshake)机制的第二个步骤。
    这里决定内核在放弃连接之前所送出的 SYN+ACK 数目。不应该大于255，默认值是5，对应于180秒
    左右时间。(可以根据上面的 tcp_syn_retries 来决定这个值)

* net.ipv4.tcp_keepalive_time ：INTEGER
    默认值是7200(2小时)
    当keepalive打开的情况下，TCP发送keepalive消息的频率。
    (由于目前网络攻击等因素,造成了利用这个进行的攻击很频繁,曾经也有cu的朋友提到过,说如果2边建立了连接,
    然后不发送任何数据或者rst/fin消息,那么持续的时间是不是就是2小时,空连接攻击?
    tcp_keepalive_time就是预防此情形的.我个人在做nat服务的时候的修改值为1800秒)

* net.ipv4.tcp_keepalive_intvl：INTEGER
    默认值为75
    探测消息发送的频率，乘以tcp_keepalive_probes就得到对于从开始探测以来没有响应的连接杀除的时间。
    默认值为75秒，也就是没有活动的连接将在大约11分钟以后将被丢弃。(对于普通应用来说,这个值有一些偏大,
    可以根据需要改小.特别是web类服务器需要改小该值,15是个比较合适的值)

* net.ipv4.tcp_keepalive_probes：INTEGER
    默认值是9
    TCP发送keepalive探测以确定该连接已经断开的次数。(注意:保持连接仅在SO_KEEPALIVE套接字选
    项被打开是才发送.
    次数默认不需要修改,当然根据情形也可以适当地缩短此值.设置为5比较合适)

* net.ipv4.tcp_retries1 ：INTEGER
    默认值是3
    放弃回应一个TCP连接请求前﹐需要进行多少次重试。RFC 规定最低的数值是3﹐这也是默认值﹐
    根据RTO的值大约在3秒 - 8分钟之间。(注意:这个值同时还决定进入的syn连接)

* net.ipv4.tcp_retries2 ：INTEGER
    默认值为15
    在丢弃激活(已建立通讯状况)的TCP连接之前﹐需要进行多少次重试。默认值为15，根据RTO的值来决定，
    相当于13-30分钟(RFC1122规定，必须大于100秒).(这个值根据目前的网络设置,可以适当地改小,我的网络内修改为了5)

* net.ipv4.tcp_orphan_retries ：INTEGER
    默认值是7
    在近端丢弃TCP连接之前﹐要进行多少次重试。默认值是7个﹐相当于 50秒 - 16分钟﹐视 RTO 而定。
    如果您的系统是负载很大的web服务器﹐那么也许需要降低该值﹐这类 sockets 可能会耗费大量的资源。另外参的考 tcp_max_orphans 。
    (事实上做NAT的时候,降低该值也是好处显著的,我本人的网络环境中降低该值为3)

* net.ipv4.tcp_max_tw_buckets ：INTEGER
    默认值是180000
    系 统在同时所处理的最大 timewait sockets 数目。
    如果超过此数的话﹐time-wait socket 会被立即砍除并且显示警告信息。之所以要设定这个限制﹐纯粹为了抵御那些简单的 DoS 攻击﹐
    千万不要人为的降低这个限制﹐不过﹐如果网络条件需要比默认值更多﹐则可以提高它(或许还要增加内存)。(事实上做NAT的时候最好可以适当地增加该值)

* net.ipv4.tcp_tw_reuse：BOOLEAN
    默认值是0
    该文件表示是否允许重新应用处于TIME-WAIT状态的socket用于新的TCP连接(这个对快速重启动某些服务,而启动后提示端口已经被使用的情形非常有帮助)

* net.ipv4.tcp_window_scaling ：INTEGER
    缺省值为1
    该 文件表示设置tcp/ip会话的滑动窗口大小是否可变。参数值为布尔值，为1时表示可变，为0时表示不可变。
    tcp/ip通常使用的窗口最大可达到 65535 字节，对于高速网络，该值可能太小，这时候如果启用了该功能，
    可以使tcp/ip滑动窗口大小增大数个数量级，从而提高数据传输的能力(RFC 1323)。
    （对普通地百M网络而言，关闭会降低开销，所以如果不是高速网络，可以考虑设置为0）
