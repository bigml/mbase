# 浏览器跨域安全问题

首先，看看浏览器上跨域安全问题的根源，为何要保护，要保护些啥？

举个例子，用户正在通过浏览器观看黄色网站A，A页面如果能够请求比特币网站B的接口，进行带B Cookie验证的比特币交易，那真是人财两空了。

为此，在浏览器诞生之初，就涉及到了 域名安全策略[Same-origin policy](https://en.wikipedia.org/wiki/Same-origin_policy) 问题，
1995 年的 Netscape Navigator 2 浏览器便设置了不允许跨域访问敏感资源。

* 何为敏感资源
same-origin policy applies only to scripts. This means that resources such as images, CSS, and
dynamically-loaded scripts, can be accessed across origins via the corresponding HTML tags (with fonts being
a notable exception).

但，同时，HTML 的 <script> 标签 are allowed to retrieve and execute content from other domains。
这道很小的口子，便成就了10年后的JSONP，这是后话。

同域安全策略虽然在浏览器侧保护了用户的隐私数据不被随意使用，但同时，也给需要跨域名共享敏感资源的网站开发人员带来了不少麻烦。
为了绕过浏览器的此策略，开发人员采用了如下对策：

## 1. JSONP
2005年，George Jempty 提出了 [JSONP](https://web.archive.org/web/20120608162347/http://bob.ippoli.to/archives/2005/12/05/remote-json-jsonp/)
通过服务器端配合，能 GET 网站B的带Cookie敏感数据。个人觉得，这种方式最为简单有效，当然，也存在一些[问题](https://en.wikipedia.org/wiki/JSONP#Security_concerns)

## 2. CORS Cross-Origin Resource Sharing
2004年，Matt Oshry, Brad Porter, and Michael Bodell 为 VoiceXML browsers 提议了 CORS 的方案，
2006年，W3C 制定了 [XMLHttpRequest](https://www.w3.org/TR/2006/WD-XMLHttpRequest-20060405/) 对象标准，
结合早先的 [Ajax](https://en.wikipedia.org/wiki/Ajax) 技术，2009年，W3C 又制定了[Cross-Origin Resource Sharing](https://www.w3.org/TR/2009/WD-cors-20090317/) 标准
至此，跨域安全问题的官方解决方案落地。时间久了，闲得蛋疼的人们又在折腾Fetch标准。。。

The CORS standard describes new HTTP headers which provide browsers a way to request remote URLs only when they have permission.
所以，CORS的关键在 HTTP 头的设置（包括浏览器端和服务器端）

CORS 需要HTTP服务器通过headers设置安全策略，然而，浏览器在请求B域的资源之前是不知道B域具体的安全设置的，
为此，preflight with an HTTP OPTIONS request method 被引入，而且，浏览器对结果会有缓存。。。

然而，不管服务端做什么设置，通过测试 Firefox 59.0.2, google chrome 78.0.3880.0 + lighttpd 1.4.15 的表现，
并没有发出 OPTION 请求，反而是每次都发了带 cookie 的请求（只要是设置了withCredentials: true 的 Ajax 请求）。
只是没有把数据正常返回给请求域（这对于B的数据安全性有何保障可言？）。

需要注意的是，CORS 有两种工作模式:

* simple model
The simple model does not allow setting custom request headers and omits cookies.
Further, only the HEAD, GET and POST request methods are supported,
and POST only allows the following MIME types: "text/plain", "application/x-www-urlencoded" and "multipart/form-data".
(Only "text/plain" was initially supported.)

* complete model
The complete model allow setting custom request headers and request with cookies. (Same as JSONP)
但只支持单个站外域名访问，不支持列表和通配符。

## 3. WebSockets
WebSockets 类似 xhr 的角色, 只是是个长连接，支持同域和跨域。
浏览器会为 websocket 连接加入 Origin: 请求头，指明来源域，服务端可对其进行白名单验证。(然并卵)

以上为标准策略，对于ActiveX object, Silverlight, Flash等专有策略，不做详解。


## 名词解释

* domain
  - protocol name, http or https...
  - host name
  - port number
