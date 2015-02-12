## tcp socket 编程细节

## Client

### tcp 协议特点

* connected socket is a stream, no edge.

### INIT

* socket( SOCK_STREAM
* fcntl( O_NONBLOCK
* setsockopt( SO_RCVTIMEO
* setsockopt( SO_SNDTIMEO
* connect( serveraddr
* setsockopt( SO_TCPDELAY

  Disable Nagle algorithm because we often send small packets. Huge gain in performance.

  Nagle 算法：

  要求一个TCP连接上最多只能有一个未被确认的小数据包，在该分组的确认到达之前，不能发送其他
  的小数据包。如果某个小分组的确认被延迟了（案例中的40ms），那么后续小分组的发送就会相应的
  延迟。也就是说延迟确认影响的并不是被延迟确认的那个数据包，而是后续的应答包。

### SEND

* ssend( fd, MSG_NOSIGNAL

  Like send(), but either fails, or return bytes sent as much as possible.

  1. MSG_NOSIGNAL

    Requests not to send SIGPIPE on errors on stream oriented sockets when the other end
    breaks the connection. The EPIPE error is still returned.

  2. return value
    - \> 0

      the number of characters sent.

    - -1 on error
      - EPIPE

        The socket was connected but the connection is now broken.

      - EAGAIN or EWOULDBLOCK

        The socket is marked non-blocking and the requested operation would block.

### RECEIVE

* recv(fd, 4, MSG_NOSIGNAL to get messagesize
  1. 4 the messagesize header (on your application)
  2. return value
    - \> 0

      the number of characters received.

    - == 0

      peer has performed orderly shutdown

    - <= 0 && errno != EAGAIN

      peer has performed orderly shutdown or error

* srecv(fd, messagesize-4, MSG_NOSIGNAL to get messages of this packet

  Like recv(), but either fails, or return bytes readed as much as possible.
  If return less than messagesize(but not fails), it's because EOF was reached,
  or, peer has performed an orderly shutdown.


### DONE

* close(fd


## Server

### INIT

* socket( SOCK_STREAM
* setsockopt( SO_REUSEADDR
* bind( ip, port
* listen( fd, fdnumbers
* setsockopt( TCP_NODELAY
* newfd = accept(fd
* fcntl( newfd, O_NONBLOCK

### SEND

* tssend( newfd, MSG_NOSIGNAL

  Like send(), with error proceessed following:

  1. MSG_NOSIGNAL

    Requests not to send SIGPIPE on errors on stream oriented sockets when the other end
    breaks the connection. The EPIPE error is still returned.

  2. return value
    - -1 on error
      - EAGAIN or EWOULDBLOCK

        A. wait for send() wouldn't block , and send again.

        B. return, and, if epool tell me send ok, send again.

      - EPIPE

        The socket was connected but the connection is now broken.


### RECEIVE

* recv(newfd, staticbuffer, MSG_NOSIGNAL

  Connected socket is a stream, no edge. so, you need divide and merge
  application packet by yourself.

  1. MSG_NOSIGNAL

    Requests not to send SIGPIPE on errors on stream oriented sockets when the other end
    breaks the connection. The EPIPE error is still returned.

  2. staticbuffer

    Avoid alloc memory for each message.

  2. return value

    - \> 0

      the number of characters received.

    - == 0

      peer has performed orderly shutdown

    - < 0 && errno != EAGAIN

      peer has performed orderly shutdown or error

* process_buffer(

  Differ with process_packet() on udp serverside receive. The buffer can be:

  - one packet
  - part of one packet
  - more than one packet
  - more than one and part of a packet


### DONE

* close( fd
* event_del(
* application_free(
