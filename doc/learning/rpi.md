### rpi zero w switch master to slave

  * sudo systemctl disable dnsmasq

  * sudo systemctl disable hostapd

  * /etc/rc.local 注释掉
    iptables-restore < /etc/iptables.ipv4.nat

  * /etc/sysctl.conf 注释掉
   net.ipv4.ip_forward=1

  * /etc/network/interface

  - master
    allow-hotplug wlan0
    iface wlan0 inet static
            address 192.168.2.1
            netmask 255.255.255.0
            network 192.168.2.0
            broadcast 192.168.2.255
            #wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf

  - slave
    allow-hotplug wlan0
    iface wlan0 inet manual
        wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf

    allow-hotplug wlan1
    iface wlan1 inet manual
        wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf

  * sudo systemctl enable dhcpcd


  * 附：

  - iptables:
    sudo iptables -t nat -A POSTROUTING -o ppp0 -j MASQUERADE
    sudo iptables -A FORWARD -i ppp0 -o wlan0 -m state --state RELATED,ESTABLISHED -j ACCEPT
    sudo iptables -A FORWARD -i wlan0 -o ppp0 -j ACCEPT
