## udp socket 编程细节

## Client

### udp 协议特点

* 业务数据包有大小限制（约64k）
* 传输无保证（无序、丢弃等）

### INIT

* socket( SOCK_DGRAM
* fcntl( O_NONBLOCK
* setsockopt( SO_RCVTIMEO
* setsockopt( SO_SNDTIMEO


### SEND

* sendto( serveraddr, MSG_DONTWAIT
  1. requestid
  2. serveraddr
  3. MSG_DONTWAIT

     Enables  non-blocking  operation;  if  the operation would block, EAGAIN is returned
     (this can also be enabled using the O_NONBLOCK with the F_SETFL fcntl(2)).

  4. return value
    - \> 0 the number of characters sent.
    - -1 on error
      - EAGAIN

        The socket is marked non-blocking and the requested operation would block.

      - EMSGSIZE

        The socket type requires that message be sent atomically, and the size of the message to be sent made this impossible.


### RECEIVE
* recv(
  1. requestid: Is this packet my expected?
  2. return value
    - \> 0 the number of characters received.
    - -1 on error
      - EAGAIN

        The socket is marked non-blocking and the requested operation would block.


### DONE

* close(fd


## Server

### INIT

* socket( SOCK_DGRAM
* setsockopt( SO_REUSEADDR
* bind( ip, port

### RECEIVE

* recvfrom( fd, &clientaddr
  1. return value
    - < 0 do nothing
    - \> 0 process_packet()

      receive one packet at most one time.

### SEND

* sendto( clientaddr
  1. requestid
  2. return value
    - < 0

      send packet failure. You can send a smaller packet to notify client.

### DONE

* close(fd
