run cltreq.cmd
bindd 0
connect 10.1.148.130 28802
sendf cltreq.b
recvft 20000 midrsp.b
parsebuf midrsp.b midrsp.p
closed
quit