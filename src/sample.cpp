#include "socktest.h"
#include "MsgBusBody.pb.h"
/*
#include <sys/shm.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <netpacket/packet.h>
*/
DECLARE_REGCMDS;

typedef struct
{
    uint64_t ddwId;
    uint16_t wSubCmd;
    uint16_t wBodyLen;
} S_MSG_H;

DEFCMDPROC(CmdHello)
{
    fprintf(pOut, "Hello!\n");
    //fprintf(pOut, "= OK\n");
    return 0;
}
#define TCPTEST "tcptest"
#define UDPTEST "udptest"

DEFCMDPROC(CmdTest)
{
    //fd = socket()
    CEthSocket oSck;
    oSck.Bind("eth1");
    char szBuff[1024];
    CSockAddrLl oSockAddr;
    int iRes = oSck.RecvFrom(szBuff, sizeof(szBuff), &oSockAddr);
    if (iRes < 0)
    {
        PWriteString("= failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("%uBytes Recvs.\n", iRes);
    
    return 0;
}

DEFCMDPROC(CmdMakeProtoBuf)
{
    char szFile[1024];
    char szTmp[64];
    char szTmp2[64];
    uint8_t acBuff[4096];
    int iLen = 0;
    uint32_t dwHonorId;
    uint32_t dwUin;
    pParser->ReadDword(dwHonorId);
    pParser->ReadDword(dwUin);
    pParser->ReadString(szFile, sizeof (szFile));

    S_MSG_H* pPkgH = (S_MSG_H*)acBuff;
    iLen += sizeof (S_MSG_H);
    pPkgH->ddwId = htonll(11223344);
    pPkgH->wSubCmd = htons(0xe028);
    BitmapProtocol oPkg;
    oPkg.set_flag_id(dwHonorId);
    oPkg.set_to_uin(dwUin);
    oPkg.set_old_value(0);
    oPkg.set_new_value(1);
    oPkg.SerializeToArray(acBuff + iLen, oPkg.ByteSize());
    pPkgH->wBodyLen = htons(oPkg.ByteSize());
    iLen += oPkg.ByteSize();
    FILE* pFile = fopen(szFile, "wb");
    fwrite(acBuff, 1, iLen, pFile);
    fclose(pFile);
}

DEFCMDPROC(CmdParseProtoBuf)
{
    uint8_t acBuff[4096];
    FILE* pFile = fopen("svrreq.b", "rb");
    S_MSG_H stPkgH;
    fread(&stPkgH, sizeof (S_MSG_H), 1, pFile);
    int iRet = fread(acBuff, 1, 4096, pFile);
    fclose(pFile);
    BitmapProtocol oPkg;
    oPkg.ParseFromArray(acBuff, iRet);
    fprintf(pOut, "dwHonorId = %d\n", oPkg.flag_id());
    fprintf(pOut, "dwHonorUin = %d\n", oPkg.to_uin());
    //fprintf(pOut, "= OK\n");
    return 0;
}

BEGIN_REGCMDS
REGCMD("test",
       CCommandInfo("test command. \n"
                    "usage: test\n"),
       CmdTest);

REGCMD("hello",
       CCommandInfo("sample command. print \"hello!\"\n"
                    "usage: hello\n"),
       CmdHello);

REGCMD("makepb",
       CCommandInfo("\n"),
       CmdMakeProtoBuf);

REGCMD("parsepb",
       CCommandInfo("\n"),
       CmdParseProtoBuf);


END_REGCMDS

