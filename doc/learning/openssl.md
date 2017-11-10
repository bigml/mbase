## openssl

###  key generation

```
    $ openssl genrsa -aes128 -out ml_rsa.key 2048             生成rsa密钥
    $ openssl rsa -text -in ml_rsa.key                        查看rsa密钥
    $ openssl rsa -in ml_rsa.key -pubout -out ml_rsa_pub.key  从密钥文件中获取公钥

    $ openssl dsaparam -genkey 2048 | openssl dsa -out ml-dsa.key -aes128  生成dsa密钥

    $ openssl ecparam -list_curves
    $ openssl ecparam -genkey -name secp256r1 | openssl ec -out ml-ecdsa.key -aes128   生成ecdsa密钥
```

### create a Certificate Signing Request

```
    $ vim ml.conf
    [req]
    prompt = no
    distinguished_name = dn
    req_extensions = ext
    input_password = 1qa2ws

    [dn]
    CN = www.imdida.org
    emailAddress = webmaster@imdida.org
    O = IMDIDA LTD
    L = ChangSha
    C = ZH

    [ext]
    subjectAltName = DNS:www.imdida.org,DNS:imdida.org
```

````
    $ openssl req -new -key ml_rsa.key -out ml_rsa.csr -config ml.conf      生成认证申请文件
    $ openssl req -text -in ml_rsa.csr -noout                               查看认证申请(csr文件自身不输出)
```

### sign your own certificates

If you are installing a TLS server for you own use, It's much easier just to use self-signed certificate.

* create certificates valid for single hostname

```
    $ openssl x509 -req -days 365 -in ml_rsa.csr -signkey ml_rsa.key -out ml_rsa.crt
```

OR, you don't actually have to create CSR in a separate step:

```
    $ openssl req -new -x509 -days 365 -key ml_rsa.key -out ml_rsa.crt
      -subj "/C=ZH/L=ChangSha/O=IMDIDA Ltd/CN=www.imdida.org"
```


* create certificates valid for multi hostnames

```
    $ vim ml.ext
    subjectAltName = DNS:*.imdida.org, DNS:imdida.org
    $ openssl x509 -req -days 365 -in ml_rsa.csr -signkey ml_rsa.key -out ml_rsa.crt -extfile ml.ext
```


* examing certificate

certificates issued by public CAs are much more interesting, as they contain a number of additional fileds.

```
    $ openssl x509 -text -in ml_rsa.crt -noout
```


### key an certificate conversion

* DER

  Binary certificate. Contains an X.509 certificate in its raw form, using DER ASN.1 encoding.

* PEM

  ASCII certificate contains a base64-encoded DER certificate with -----BEGIN CERTIFICATE----- used as the header and -----END CERTIFICATE----- as the footer.


```
    $openssl x509 -inform PEM -in ml_rsa.crt -outform DER -out ml_rsa.der
```



### 配置

### 密钥协商参数

```
    $ openssl dhparam -out dhparam.pem 2048
```


#### cipher suite selection


[recommend](https://wiki.mozilla.org/Security/Server_Side_TLS#Recommended_configurations)
[SSL/TLS CipherSuite 介绍](https://blog.helong.info/blog/2015/01/23/ssl_tls_ciphersuite_intro/)

一个CipherSuite是4个算法的组合：

* 1个authentication (认证)算法
* 1个encryption(加密)算法
* 1个message authentication code (消息认证码 简称MAC)算法
* 1 个key exchange(密钥交换)算法

Ciphersuites: ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256

Versions: TLSv1.2
TLS curves: prime256v1, secp384r1, secp521r1
Certificate type: ECDSA
Certificate curve: prime256v1, secp384r1, secp521r1
Certificate signature: sha256WithRSAEncryption, ecdsa-with-SHA256, ecdsa-with-SHA384, ecdsa-with-SHA512
RSA key size: 2048 (if not ecdsa)
DH Parameter size: None (disabled entirely)
ECDH Parameter size: 256
HSTS: max-age=15768000
Certificate switching: None



安全分析的第一步是建立攻击模型，TLS的攻击模式是：

* 攻击者有充足的计算资源
* 攻击者无法得到私钥，无法得到客户端和服务器内存里面的密钥等保密信息
* 攻击者可以抓包，修改包，删除包，重放包，篡改包。

这个模型其实就是密码学里面一般假定的攻击模型。


TLS的主要实现：

OpenSSL
libressl
boringssl(Google)
libressl
s2n(Amazon)
nss(Mozilla)
polarssl
botan
gnutls(gpl)
cyassl
go.crypto


ARP         ip => mac address route protocol
libpcap     网络数据包捕获函数库




密码学通信协议也是分层构造得到。大致可以这么分层：

最底层是基础算法原语的实现，例如: aes , rsa， md5, sha256，ecdsa, ecdh 等（举的例子都是目前的主流选择，下同）

其上是选定参数后，符合密码学里标准分类的算法，包括块加密算法，签名算法，非对称加密算法，MAC算法等，例如：
aes-128-cbc-pkcs7，rsaes-oaep ，rsassa-pkcs1-v1_5, hmac-sha256，ecdsa-p256，curve25519 等

再其上，是把多种标准算法组合而成的半成品组件，例如：对称传输组件例如 aes-128-cbc + hmac-sha256，aes-128-gcm，
认证密钥协商算法: rsassa-OAEP + ecdh-secp256r1，数字信封：rsaes-oaep + aes-cbc-128 + hmac-sha256 ，文件密码
加密存储组件：pbkdf2+aes-128-cbc-hmac-sha256，密钥扩展算法 PRF-sha256 等

再其上，是用各种组件拼装而成的各种成品密码学协议/软件，例如：tls协议，ssh协议，srp协议，gnupg文件格式，
iMessage协议，bitcoin协议等等



第2层中，密码学算法，常见的有下面几类：

块加密算法 block cipher: AES, Serpent, 等
流加密算法 stream cipher: RC4，ChaCha20 等
Hash函数 hash funtion:MD5，sha1，sha256，sha512 , ripemd 160，poly1305 等
消息验证码函数 message authentication code: HMAC-sha256，AEAD 等
密钥交换 key exchange: DH，ECDH，RSA，PFS方式的（DHE，ECDHE）等
公钥加密 public-key encryption: RSA，rabin-williams 等
数字签名算法 signature algorithm:RSA，DSA，ECDSA (secp256r1 , ed25519) 等
密码衍生函数 key derivation function: TLS-12-PRF(SHA-256) , bcrypto，scrypto，pbkdf2 等
随机数生成器 random number generators: /dev/urandom 等
