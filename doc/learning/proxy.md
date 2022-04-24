## 代理

### git

1. http 代理

    https://github.com/nanmi/YOLOX-deepstream.git

此类代理使用 ~/.gitconfig 即可：

```txt
[http]
    proxy = http://172.16.0.121:7890
```


2. ssh 代理

    git@github.com:dodoma/reef.git

此类代理需结合 ssh 代理才能正常使用。

```
ssh git@github.com  -o "ProxyCommand=nc -X connect -x 172.16.0.121:7890 %h %p"
```

提示能够正常登陆 github.com 才可继续，修改 .ssh/config:

```
Host codea
	Hostname txa
	Port 54370

Host github-bigml
  HostName github.com
  User bigml
  IdentityFile /home/ml/.ssh/id_rsab
  IdentitiesOnly yes
  ProxyCommand nc -X connect -x 172.16.0.121:7890 %h %p
```
