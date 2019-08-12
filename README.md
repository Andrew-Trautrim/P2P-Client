# P2P-Client
Peer to peer client written in C

A P2P client works by created a connection over the internet directly from computer to computer (or peer to peer). The connection is done without the need for a central server as each connected device acts as both server and client to maintain the network. Once a connection is created, devices can send and recieve messages, run commands, and search directories.

Command line usage:
<pre>
root@linux:~/P2P-Client# ./bin/session 
Improper usage, use '-l' and/or '-a' to connect
type '-h' for help
root@linux:~/P2P-Client/bin# ./session -h
usage: ./session &ltoptions&gt

options:
	-a address : connect to target address
	-h	   : display help options
	-l	   : listen for incoming connections
	-n max #   : maximum number of incoming connections
	-p port	   : local port for accepting connections
	-t port    : target port for connecting
</pre>

Program execution:  
listening for incoming connections,
<pre>
root@linux:~/P2P-Client/bin# ./session -l -n 5 -p 18
Listening on port 18
Listening on port 19
Listening on port 20
Listening on port 21
Listening on port 22
Session (type 'X' to exit): 
78.69.87.84 connected on port 20
What makes you think she's a witch?
[78.69.87.84] She turned me into a newt.
A newt?
[78.69.87.84] ...I got better.
[!] 78.69.87.84 disconnected
</pre>
connect to target address,
<pre>
root@linux:~/P2P-Client/bin# ./session -a 10.0.58.1 -t 20
Connected to 87.73.83.69
Session (type 'X' to exit): 
[87.73.83.69] What makes you think she's a witch?
She turned me into a newt.
[87.73.83.69] A newt?
...I got better.
X
</pre>
