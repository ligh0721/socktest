stPkgH = cFlag + wLength@
stReqH = stPkgH + wVersion@ + wCommand@ + dwSequence@ + dwUin@ + dwUserIp@ + dwReserved@
stWBuff = wBuffLen@ + acBuff[wBuffLen@]
stReqB = dwFromUin@ + dwFriendUin@
stPack = stReqH + stReqB + stWBuff + cEtx03
@patch stPack.stReqH.stPkgH.wLength@ = stPack.dwLENGTH

