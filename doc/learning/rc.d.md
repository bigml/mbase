# System V init

## Write an Init script that conforms to LSB Init Standard.

Init scripts are used to start|stop a software|service. For example, if you are using postgresql software,
we will have a Init script named ‘/etc/init.d/postgresql’ which can be used to
‘start|stop|restart|reload|force-reload|status’.

LSB-compliant init scripts need to:

* Provide at-least ‘start, stop, restart, force-reload, and status’
* Return Proper exit code
* Document run-time dependencies



## 方案A

1. Copy the /etc/init.d/skeleton file (to e.g. /etc/init.d/rajeevdaemon or another good name), which is a shell
script with a lot of comments, and edit it to suit your needs.

2. Add appropriate symlinks from e.g. /etc/rc2.d/S98rajeevdaemon and /etc/rc2.d/K98rajeevdaemon to it.
or
3. update-rc.d filename defaults


## 方案B

[脚本实例](my_daemon)


# systemd

Oh, 现如今，这一堆的 shell 脚本，已经被 systemd 的 /lib/systemd/system/*.service 无情的取代了。

$ cp usb-notify@.service /lib/systemd/system
$ sudo systemctl enable usb-notify@.service
$ sudo systemctl daemon-reload
$ systemctl status media-sda1.mount


### 默认列出所有 Unit
$ systemctl ,or, systemctl list-units

### 查看当前系统状态
$ systemctl status

### 列出所有被激活的服务
$ systemctl list-units -t service

### 查看启动时的默认 Target
$ systemctl get-default

### 查看某个 Target 内所有的 Unit
systemctl list-dependencies multi-user.target

### 设置启动时的默认 Target
$ sudo systemctl set-default multi-user.target

### 获取当前启动级别
$ systemctl get-default
$ ls -l /etc/systemd/system/default.target
$ rm /etc/systemd/system/default.target; ln -s /lib/systemd/system/ftv.target /etc/systemd/system/default.target

### 分析系统启动时间
$ systemd-analyze 打印启动时间（kernel + User）
$ systemd-analyze blame 打印所有正在运行的 units，按时间排序
$ systemd-analyze critical-chain 同 blame ，树状显示
$ systemd-analyze plot > bootup.svg 方便图形查看

###
$ systemctl list-unit-files --type=target
$ systemctl list-dependencies [--all] lighttpd.service OR rescue.target
$ systemctl cat systemd-remount-fs


### 启动 moc
$ systemctl start moc
$ systemctl status moc

### 默认启动 moc
$ systemctl enable moc

# journalctl -u moc -f 实时查看 moc 的日志
