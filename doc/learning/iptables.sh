#!/bin/bash
#0 0 * * * /root/start_iptables.sh
#清除配置
/sbin/iptables -P INPUT ACCEPT
/sbin/iptables -F
/sbin/iptables -X
#开放本地和Ping
/sbin/iptables -A INPUT -i lo -j ACCEPT
/sbin/iptables -A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
/sbin/iptables -A INPUT -p icmp -j ACCEPT
#配置内网白名单
/sbin/iptables -A INPUT -s 10.0.0.0/8 -j ACCEPT
/sbin/iptables -A INPUT -s 172.16.0.0/12 -j ACCEPT
/sbin/iptables -A INPUT -s 192.168.0.0/16 -j ACCEPT
#配置外网白名单
/sbin/iptables -A INPUT -s 180.168.36.198 -j ACCEPT
/sbin/iptables -A INPUT -s 180.168.34.218 -j ACCEPT
/sbin/iptables -A INPUT -s 222.73.202.251 -j ACCEPT
#控制端口
/sbin/iptables -A INPUT -p tcp --dport 80 -j ACCEPT
/sbin/iptables -A INPUT -p tcp --dport 22 -j ACCEPT
#拒绝其它
/sbin/iptables -A INPUT -j DROP
/sbin/iptables -A FORWARD -j DROP
#开放出口
/sbin/iptables -A OUTPUT -j ACCEPT



chmod 755 /root/start_iptables.sh
crontab -e
0 0 * * * /root/start_iptables.sh
