/* 
 * File:   UdpTestMain.cpp
 * Author: thunderliu
 *
 * Created on 2011年5月26日, 下午6:38
 */

#include "stdinc.h"
#include "Section.h"
#include "CommCmd.h"


CUdpSocket g_oUdpSock;


/*
DEFCMDPROC(CmdCreate)
{
    if (g_oUdpSock.Create())
    {
        fprintf(pOut, "= OK\n");
    }
    else
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
    }
    return 0;
}
 */

DEFCMDPROC(CmdClose)
{
    if (!g_oUdpSock.Close())
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK\n");
}

DEFCMDPROC(CmdBind)
{
    int iPort;
    fscanf(pIn, "%d", &iPort);
    if (!g_oUdpSock.Bind(NULL, iPort))
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %d\n", iPort);
}

DEFCMDPROC(CmdSendToStr)
{

    int iRet;
    char sStr[1024];
    int iPort;
    fscanf(pIn, "%s%d", sStr, &iPort);
    CSockAddr oSockAddr(sStr, iPort);
    //CUdpSocket::MakeSockAddr(&stSockAddr, "127.0.0.1", iPort);
    fscanf(pIn, " %[^\r\n]", sStr);
    iRet = g_oUdpSock.SendTo(sStr, strlen(sStr), oSockAddr);
    if (-1 == iRet)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %d\n", iRet);
}

DEFCMDPROC(CmdRecvFromStr)
{
    int iRet;
    char sStr[1024];
    bzero(sStr, sizeof (sStr));
    CSockAddr oSockAddr;
    iRet = g_oUdpSock.RecvFrom(sStr, 128, oSockAddr);
    if (iRet <= 0)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %s:%d %s\n", oSockAddr.GetAddr(), oSockAddr.GetPort(), sStr);
}

DEFCMDPROC(CmdSendToFile)
{
    int iRet;
    int iSize = 0;
    char szStr[4096];
    int iPort;
    fscanf(pIn, "%s%d", szStr, &iPort);
    CSockAddr oSockAddr(szStr, iPort);
    fscanf(pIn, " %[^\r\n]", szStr);
    FILE* pFile = fopen(szStr, "rb");
    if (NULL == pFile)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    while (!feof(pFile))
    {
        iRet = fread(szStr, 1, 4096, pFile);
        if (iRet < 0)
        {
            fprintf(pOut, "= Failed | %s\n", strerror(errno));
            break;
        }
        iSize += iRet;
        iRet = g_oUdpSock.SendTo(szStr, iRet, oSockAddr);
        if (iRet < 0)
        {
            fprintf(pOut, "= Failed | %s\n", strerror(errno));
            break;
        }
    }
    fclose(pFile);
    fprintf(pOut, "= OK | %d\n", iSize);
}

DEFCMDPROC(CmdRecvFromFileT)
{
    int iRet;
    int iSize = 0;
    int iTimeout;
    char sStr[4096];
    //bzero(sStr, sizeof (sStr));
    fscanf(pIn, "%d", &iTimeout);
    fscanf(pIn, " %[^\r\n]", sStr);
    FILE* pFile = fopen(sStr, "wb");
    if (NULL == pFile)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    CSockAddr oSockAddr;
    g_oUdpSock.SetRecvTimeout(iTimeout);
    while ((iRet = g_oUdpSock.RecvFrom(sStr, 4096, oSockAddr)) > 0)
    {
        iRet = fwrite(sStr, 1, iRet, pFile);
        if (iRet < 0)
        {
            break;
        }
        iSize += iRet;
    }
    fclose(pFile);
    if (iRet < 0)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %s:%d %d\n", oSockAddr.GetAddr(), oSockAddr.GetPort(), iSize);
}

DEFCMDPROC(CmdRecvFromStrT)
{
    struct sockaddr_in stSockAddr = {0};
    int iRet;
    char sStr[1024];
    int iTimeout;
    bzero(sStr, sizeof (sStr));
    fscanf(pIn, "%d", &iTimeout);
    g_oUdpSock.SetRecvTimeout(iTimeout);
    CSockAddr oSockAddr;
    iRet = g_oUdpSock.RecvFrom(sStr, 128, oSockAddr);
    if (iRet < 0)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %s:%d %s\n", oSockAddr.GetAddr(), oSockAddr.GetPort(), sStr);
}

/*
DEFCMDPROC(CmdCmdSendToLargeFile)
{
    struct sockaddr_in stSockAddr = {0};
    int iRet;
    int iSize = 0;
    char sStr[4096];
    int iPort;
    fscanf(pIn, "%s%d", sStr, &iPort);
    CSockAddr oSockAddr(sStr, iPort);
    fscanf(pIn, " %[^\r\n]", sStr);
    
    
    FILE* pFile = fopen(sStr, "rb");
}

DEFCMDPROC(CmdCommand)
{
    fprintf(pOut, "= OK\n");
    CCmdParser oParser;
    oParser.SetPrompt("Cmd>");
    oParser.RegCmd("quit", CmdQuit);
    
    oParser.ParseLoop(stdin, stdout);
}

DEFCMDPROC(CmdCommandDaemon)
{
    fprintf(pOut, "= OK\n");
}

/*
int convToProtobuf(void *data, int *len)
{
    RelationUpdateFromSns testMessage;
    //填充包头
    char wbhead[20];
    WeiboPkgHeadExt *whead = testMessage.mutable_weibo_pkg_head_ext();
    whead->set_length(sizeof(wbhead));
    whead->set_body(wbhead, sizeof(wbhead));

    //填写包体
    testMessage.set_from(1);
    testMessage.set_type(2);
    testMessage.set_time(0);
    testMessage.set_uin(174209756);
    //填写id列表
    testMessage.add_friend_uin_list(1122334455);
    testMessage.add_friend_uin_list(887766);
    testMessage.add_friend_uin_list(1385000);

    testMessage.SerializeWithCachedSizesToArray((uint8_t *)data);
 *len = testMessage.ByteSize();
    return 0;
}

typedef struct
{
    uint64_t ddwId;
    uint16_t wType;
    uint16_t wLength;
} S_TLV_H;

DEFCMDPROC(CmdMakeProtoBuf)
{
    char acBuf[4096];
    int iSize = 4096;
    S_TLV_H stTlv = {123456, 0xf701};
    convToProtobuf(acBuf, &iSize);
    stTlv.wLength = iSize;
	
    RelationUpdateFromSns oRufs;
    int iRet = oRufs.ParseFromArray(acBuf, iSize);
    if (iRet < 0)
    {
        fprintf(pOut, "= Failed | ParseFromArray\n");
        return;
    }
    if (!oRufs.has_type())
    {
        fprintf(pOut, "= Failed | has_type\n");
        return;
    }
    const uint8_t cType = (uint8_t)oRufs.type();
    if (2 != cType)
    {
        fprintf(pOut, "= Failed | cType != 0xf701\n");
        return;
    }

    if (!oRufs.has_uin())
    {
        fprintf(pOut, "= Failed | has_uin\n");
        return;
    }
    const uint32_t dwUin = oRufs.uin();
    fprintf(pOut, "= OK | %d\n", dwUin);

    const uint16_t wFriendUinNum = (uint16_t)oRufs.friend_uin_list_size();
	
    FILE* pFile = fopen("test.buf", "wb");
    fwrite(&stTlv, 1, sizeof(S_TLV_H), pFile);
    fwrite(acBuf, 1, iSize, pFile);
    fprintf(pOut, "= OK | test.buf\n");
    fclose(pFile);
}
 */



DEFCMDPROC(CmdTest)
{
    CFileMemMap oMm;
    if (!oMm.Create("test.txt", false))
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    memmove(oMm, "Hello Memory Map!", 18);
    fprintf(pOut, "= OK\n");
}

int main(int argc, char** argv)
{
    g_iArgc = argc;
    g_ppArgv = argv;
    RegCmds();
    g_oParser.RegCmd("test", CmdTest);
    g_oParser.RegCmd("close", CmdClose);
    g_oParser.RegCmd("bind", CmdBind);
    g_oParser.RegCmd("sendtos", CmdSendToStr);
    g_oParser.RegCmd("recvfroms", CmdRecvFromStr);
    g_oParser.RegCmd("sendtof", CmdSendToFile);
    g_oParser.RegCmd("recvfromft", CmdRecvFromFileT);
    g_oParser.RegCmd("recvfromst", CmdRecvFromStrT);

    //g_oUdpSock.Create();
    /*
    int i;
    for (i = 1; i < argc; i++)
    {
        g_oParser.ParseLine(stdin, stdout);
    }
     */
    if (argc >= 2)
    {
        FILE* pFile = fopen(argv[1], "r");
        if (pFile == NULL)
        {
            fprintf(stdout, "= Failed | %s\n", strerror(errno));
            RETURN_ERR(-1);
        }
        g_oParser.ParseLoop(pFile, stdout, argc, argv);
        fclose(pFile);
        return 0;
    }
    g_oParser.ParseLoop(stdin, stdout, argc, argv);


    return 0;
}
