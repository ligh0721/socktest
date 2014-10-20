#!/usr/local/bin/socktest

bindl %1
listen
accept
recvf %2 500
closed
closel
quit
