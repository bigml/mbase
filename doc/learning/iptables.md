-t table (filter, nat)

-i interface name

-I chain (INPUT, FORWARD, OUTPUT)  (PREROUTING, INPUT, OUTPUT, POSTROUTING)

-s source specification

   -s ! 192.168.100.0/24 表示不许 192.168.100.0/24 之封包来源；


-d dest specification

-p tcp --(s/d)port 80  (tcp, udp, icmp, all)

-j target (ACCEPT, DROP, REJECT, LOG to /var/log/messages)

-n fast
-v verbose
-t


-F [chain]		Delete all rules in  chain or all chains
-Z [chain]      将所有的 chain 的计数与流量统计都归零


iptables -I INPUT  -s 119.39.101.84 -p tcp --dport 80 -j REJECT
iptables -nL --line-number
iptables-save dump iptables rules to stdout


* 取消 ICMP 8 类型的封包回应
echo "1" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts




#iptables 外挂模块：mac 与 state
    iptables -A INPUT [-m state] [--state 状态]
选项与参数：
-m ：一些 iptables 的外挂模块，主要常见的有：
     state ：状态模块
     mac   ：网络卡硬件地址 (hardware address)
--state ：一些封包的状态，主要有：
     INVALID    ：无效的封包，例如数据破损的封包状态
     ESTABLISHED：已经联机成功的联机状态；
     NEW        ：想要新建立联机的封包状态；
     RELATED    ：这个最常用！表示这个封包是与我们主机发送出去的封包有关
#ICMP 封包规则的比对：针对是否响应 ping 来设计
    iptables -A INPUT [-p icmp] [--icmp-type 类型] -j ACCEPT
选项与参数：
--icmp-type ：后面必须要接 ICMP 的封包类型，也可以使用代号，
              例如 8  代表 echo request 的意思。
