### ssh 简直是神一般的存在

代理其实就是一个中介，A和B想直连，中间插入一个C，C就是中介。刚开始的时候，代理多数是帮助内网client访问
外网server用的（比如HTTP代理），从内到外。后来出现了反向代理，”反向”这个词在这儿的意思其实是指方向相反，即代
理将来自外网client的请求forward到内网server，从外到内。


ssh 至隐藏在各种路由器后面的伪公网物联网机器时，就需要这样的反向代理。

ssh -R 选项专门用来干这种勾当，ngrok也是基于此。


A. 在物联网机器上执行

$ssh -R [0.0.0.0/*:]port:host:hostport usera@bind_address

  bind_address 和 port 为公网 ip 和端口

  host 和 hostport（一般为22） 为本机（物联网机）需要转发的端口

  usera 为 bind_address 机器上的用户

B. 其他机器随时登陆

$ ssh userb@bind_address -p port

  userb 为物联网机器的用户

注意：

  1. bind_address 机器必须是公网机器，需要开放port的外网访问，若port < 1024，usera 必须为 root

  2. 0.0.0.0 或 * 表示代理机器绑定的网卡
     反向代理默认只开放给内网的用户，若需要绑定至所有网卡，需配置 /etc/ssh/sshd_config GatewayPorts yes



e.g.

$ ssh -NTfR 54110:localhost:22 demo@txa

-N ssh不执行命令
-T 不分配TTY
-f 后台执行
-R 反向代理

-o ServerAliveInterval=1800

N参数，表示只连接远程主机，不打开远程shell；T参数，表示不为这个连接分配TTY。这个两个参数可以放在一起用，代表这个SSH连接只用来传数据，不执行远程操作。
f参数，表示SSH连接成功后，转入后台运行。这样一来，你就可以在不中断SSH连接的情况下，在本地shell中执行其他操作。


参考
netcan
ssh隧道反向代理实现内网到公网端口转发
http://www.netcan666.com/2016/09/28/ssh%E9%9A%A7%E9%81%93%E5%8F%8D%E5%90%91%E4%BB%A3%E7%90%86%E5%AE%9E%E7%8E%B0%E5%86%85%E7%BD%91%E5%88%B0%E5%85%AC%E7%BD%91%E7%AB%AF%E5%8F%A3%E8%BD%AC%E5%8F%91/


#### gitolite Host key verification failed. read_passphrase: can't open /dev/tty

有时候，我们的gitolite仓库hook里面需要rsync文件(比如提交后自动编译部署，参考post-receive)，
首次运行时，会提示如上信息，无法与目标服务器建立连接，原因在于

1. You can't connect using interactive SSH if you don't have a terminal to read the password from. What exactly
   are you trying to achieve? If you need to connect via SSH without typing a password, you'll need RSA keys as
   explained here or here, but do use a passphrase and ssh-agent. Once you've done that, you can disable
   password authentication in SSH entirely, thus fortifying your installation. And disabling root in
   sshd_config is always a good idea.

2. The Host key verification failed error message means that your SSH client has compared the received
   public key of the remote server and noticed that it does not match the stored version of the host key in
   ~/.ssh/known_hosts file.

   The default value for the StrictHostKeyChecking option is ask, which prompts you to accept or reject
   previously-unknown host keys... but since your session apparently has no pseudo-TTY, the ssh client is in
   non-interactive mode and won't prompt. The value no should auto-accept the host key: if possible, ssh
   will write the host key to ~/.ssh/known_hosts file. If this is successful, you'll need to use the -o
   StrictHostKeyChecking=no option only when connecting to a particular server for the first time.

reference:

[a](https://unix.stackexchange.com/questions/416166/cant-establish-ssh-connection-debug1-read-passphrase-cant-open-dev-tty-n)

[b](https://www.centos.org/forums/viewtopic.php?t=53265)
