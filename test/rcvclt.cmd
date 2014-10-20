closel
bindl 28802
listen
accept
closel
recvft 100000 cltreq.b
parsebuf cltreq.b cltreq.p
run sndsvr.cmd
closed
quit