# P2P-Client
Peer to peer client written in C

A P2P client works by created a connection over the internet directly from computer to computer (or peer to peer). The connection is done without the need for a central server as each connected device acts as both server and client to maintain the network. Once a connection is created, devices can send and recieve messages, run commands, and search directories.

Command line usage:
<pre>
root@linux:~/P2P-Client# ./bin/session 
Improper usage, use '-l' and/or '-a' to connect
type '-h' for help
root@linux:~/P2P-Client# ./bin/session -h
usage: ./session <options>

options:
	-a a1,a2,...,an : connect to target addresses
	-h              : display help options
	-l              : listen for incoming connections
	-n #            : maximum number of incoming connections
	-p p1,p2,...,pn : local ports for accepting connections
	-t p1,p2,...,pn : target ports for connecting
</pre>

Program execution:  
listening for incoming connections,
<pre>
root@linux:~/P2P-Client# ./bin/session -l -p 18,22,80,52
Listening on port 18
Listening on port 22
Listening on port 80
Listening on port 52
Chat Room, type 'X' to exit: 
port 18 connected
[78.69.87.84:18] She's a witch!
what makes you think she's a witch?
[78.69.87.84:18] She turned me into a newt.
A newt?
[78.69.87.84:18] ...I got better
[!] 78.69.87.84 disconnected
</pre>
connect to target address,
<pre>
root@linux:~/P2P-Client/bin# ./session -a 10.0.58.1 -t 20
Connected to 87.73.83.69:18
Chat Room, type 'X' to exit:
She's a witch!
[87.73.83.69:18] what makes you think she's a witch?
She turned me into a newt.
[87.73.83.69:18] A newt?
...I got better
X
</pre>
