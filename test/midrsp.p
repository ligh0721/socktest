stPkgH = cFlag + wLength@
stRspH = stPkgH + wVersion@ + wCommand@ + dwSequence@ + dwUin@ + dwReserved@ + cResult
stRspB = dwPass@
stPack = stRspH + stRspB + cEtx03
@patch stPack.stRspH.stPkgH.wLength@ = stPack.dwLENGTH
