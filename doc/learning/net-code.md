#### C Socket Client-Server  Background ####

[socket basic](http://beej.us/guide/bgnet/output/html/multipage/clientserver.html)


#### RECEIVE SOCKET TIMEOUT ####

The recv call is used to receive messages from a socket, and may be used to receive data
on a socket whether or not it is connection-oriented.The recv() function shall return the
length of the message written to the buffer pointed to by the buffer argument.

If no messages are available at the socket and O_NONBLOCK is not set on the socket’s file
descriptor, recv() shall block until a message arrives.

However sometimes application needs its thread to timeout on a socket that blocks when
reading. We can set timeout for a blocking rcv ( ) call using setscokopt function call.

setsockopt (connfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof tv);

Here SO_RCVTIMEO allow us to place a timeout on socket receives. Timeout value can be
specified in timeval structure, this let us specify the timeout in seconds and
microseconds. We can disable timeout by setting its value to 0 seconds and 0
microsecond. Both timeout are disabled by default. The receive timeout affects the five
input functions.read, readv,rcv,recvfrom,recvmsg.


#### RETURN VALUE of rcv () ####

Upon successful completion, recv() shall return the length of the message in bytes. If no
messages are available to be received and the peer has performed an orderly shutdown,
recv() shall return 0. Otherwise, -1 shall be returned and errno set to indicate the
error.


#### send, sendto, sendmsg ####

* RETURN
    the number of charactors sent, on error, -1    send(), sendto(), sendmsg() Locally
    detected errors are indicated by a return value of -1.

* errno
    EISCONN, sendto() used on a connection-mode socket, and dest_addr, addrlen parameter
    not null and 0.

    ENOTCONN, sendto(), send() used on a connection-mode socket, and socket was not
    actualy connected.

    EMSGSIZE, msg is too long to pass through underlying protocol. and the msg not
    transmitted.

    EWOULDBLOCK, msg does not fit into the send buffer of the socket. sendxxx() normaly
    blocks, unless the socket has been placed in nonblocking I/O mode. select may be used
    to determine when it is possible to send more data.

* flags
    MSG_DONTWAIT, enable nonblocking operation. This can also be enabled using the
    O_NONBLOCK flag with the F_SETFL fnctl()

    MSG_MORE(2.6), the caller has more data to send. same effect as the TCP/UDP_CORK
    socket option. this informs the kernel to package all of the data sent in calls with
    this flag set into a single datagram which is only transmited when the call is
    performed that does not specify this flag.

    MSG_NOSIGNAL, not to send SIGPIPE on errors on STREAM orinted sockets when the other
    end breaks the connection. the EPIPE error is still returned. on the other hand,
    recvxxx() return 0 when the other end breaks the connection.

    MSG_OOB, send out-of-band (urgent) data



#### recv ####
* EAGAIN
    The socket is marked non-blocking and the receive operation would block, or a receive
    timeout had been set and the  timeout  expired  before data was received.
