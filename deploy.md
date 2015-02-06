# 软件包

## 软件源安装

* git
* libevent

## 源代码安装

* clearsilver
* libmemcached
* libjsonc

* libmongo-client

* fcgi
* libmarkdown

# 安装

* clearsilver
    `git clone https://github.com/bigmaliang/clearsilver.git clearsilver`
     `
     ./autogen.sh --prefix=/usr/local \
                  --disable-wdb  \
                  --disable-perl \
                  --disable-ruby \
                  --disable-java \
                  --disable-python
     `
    `make && make install`
    `test -e "/usr/local/include/ClearSilver/ClearSilver.h"`

* libmemcached
    `wget https://launchpad.net/libmemcached/1.0/0.53/+download/libmemcached-0.53.tar.gz` recommended version

    `wget https://launchpad.net/libmemcached/1.0/1.0.18/+download/libmemcached-1.0.18.tar.gz`
    `mkdir libmemcached`
    `tar zxvf libmemcached-0.53.tar.gz -C libmemcached/ --strip-components=1`
    `./configure --prefix=/usr/local`
    `make && make install`

    `pkg-config --cflags libmemcached > /dev/null 2>&1`

* libjsonc
    `git clone https://github.com/jehiah/json-c.git libjsonc`
    `./autogen.sh && ./configure --prefix=/usr/local`
    `make && make install`

    `pkg-config --cflags json > /dev/null 2>&1`

* libmongo-client
    `git clone https://github.com/bigmaliang/libmongo-client.git libmongo-client`
    `autoreconf -i && ./configure -- prefix=/usr/local`
    `make && make install`

    `pkg-config --cflags libmongo-client > /dev/null 2>&1`

* fcgi
    `git clone https://github.com/bigmaliang/fcgi.git fcgi`
    `./configure --prefix=/usr/local`
    `make && make install`

    `test -e "/usr/local/lib/libfcgi.a"`

* libmarkdown
    `git clone https://github.com/Orc/discount.git libmarkdown`
    `./configure.sh --prefix=/usr/local`

    `test -e "/usr/local/lib/libmarkdown.a"`
