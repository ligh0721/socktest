closed
connect 10.1.148.130 28801
run midreqb.cmd
sendf midreq.b
recvft 100000 svrrsp.b
parsebuf svrrsp.b svrrsp.p
closed
quit