## CLOSE_WAIT

CLOSE_WAIT means that the local end of the connection has received
a FIN from the other end, but the OS is waiting for the program at the
local end to actually close its connection.

The problem is your program running on the local machine is not closing
the socket. It is not a TCP tuning issue. A connection can (and quite
correctly) stay in CLOSE_WAIT forever while the program holds the
connection open.

Once the local program closes the socket, the OS can send the FIN to
the remote end which transitions you to LAST_ACK while you wait for
the ACK of the FIN. Once that is received, the connection is finished
and drops from the connection table (if you're end is in CLOSE_WAIT
you do _not_ end up in the TIME_WAIT state).



LISTEN
(server) represents waiting for a connection request from any remote TCP and port.
SYN-SENT
(client) represents waiting for a matching connection request after having sent a connection request.
SYN-RECEIVED
(server) represents waiting for a confirming connection request acknowledgment after having both received and sent a connection request.
ESTABLISHED
(both server and client) represents an open connection, data received can be delivered to the user. The normal state for the data transfer phase of the connection.
FIN-WAIT-1
(both server and client) represents waiting for a connection termination request from the remote TCP, or an acknowledgment of the connection termination request previously sent.
FIN-WAIT-2
(both server and client) represents waiting for a connection termination request from the remote TCP.
CLOSE-WAIT
(both server and client) represents waiting for a connection termination request from the local user.
CLOSING
(both server and client) represents waiting for a connection termination request acknowledgment from the remote TCP.
LAST-ACK
(both server and client) represents waiting for an acknowledgment of the connection termination request previously sent to the remote TCP (which includes an acknowledgment of its connection termination request).
TIME-WAIT
(either server or client) represents waiting for enough time to pass to be sure the remote TCP received the acknowledgment of its connection termination request. [According to RFC 793 a connection can stay in TIME-WAIT for a maximum of four minutes known as two MSL (maximum segment lifetime).]
CLOSED
(both server and client) represents no connection state at all.
