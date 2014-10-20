/* 
 * File:   UdpTestMain.cpp
 * Author: thunderliu
 *
 * Created on 2011年5月26日, 下午6:38
 */

#include "stdinc.h"
#include "Socket.h"
#include "CmdParser.h"
#include "Section.h"
#include "UinFile.h"
#include "CommCmd.h"


CTcpSocket g_oLstnSock;
CTcpSocket g_oDataSock;


DEFCMDPROC(CmdCloseL)
{
    if (!g_oLstnSock.Close())
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK\n");
}

DEFCMDPROC(CmdCloseD)
{
    if (!g_oDataSock.Close())
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK\n");
}

DEFCMDPROC(CmdBindL)
{
    int iPort;
    fscanf(pIn, "%d", &iPort);
    if (!g_oLstnSock.Bind(NULL, iPort))
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %d\n", iPort);
}

DEFCMDPROC(CmdBindD)
{
    int iPort;
    fscanf(pIn, "%d", &iPort);
    if (!g_oDataSock.Bind(NULL, iPort))
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %d\n", iPort);
}

DEFCMDPROC(CmdAccept)
{
    if (!g_oLstnSock.Accept(g_oDataSock))
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %s:%d\n", g_oDataSock.GetSockAddrHandle().GetAddr(), g_oDataSock.GetSockAddrHandle().GetPort());
}

DEFCMDPROC(CmdListen)
{
    if (!g_oLstnSock.Listen(5))
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK\n");
}

DEFCMDPROC(CmdConnect)
{
    char szStr[4096];
    int iPort;
    fscanf(pIn, "%s%d", szStr, &iPort);
    if (!g_oDataSock.Connect(szStr, iPort))
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK\n");
}

DEFCMDPROC(CmdSendStr)
{
    
    int iRet;
    char szStr[1024];
    fscanf(pIn, " %[^\r\n]", szStr);
    iRet = g_oDataSock.Send(szStr, strlen(szStr));
    if (iRet <= 0)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %d\n", iRet);
}

DEFCMDPROC(CmdRecvStr)
{
    int iRet;
    char szStr[1024];
    bzero(szStr, sizeof (szStr));
    iRet = g_oDataSock.Recv(szStr, 128);
    if (iRet <= 0)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %s\n", szStr);
}

DEFCMDPROC(CmdSendFile)
{
    int iRet;
    int iSize = 0;
    char szStr[4096];
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
        iRet = g_oDataSock.Send(szStr, iRet);
        if (iRet < 0)
        {
            fprintf(pOut, "= Failed | %s\n", strerror(errno));
            break;
        }
    }
    fclose(pFile);
    fprintf(pOut, "= OK | %d\n", iSize);
}

DEFCMDPROC(CmdRecvFileT)
{
    int iRet;
    int iSize = 0;
    int iTimeout;
    char szStr[4096];
    //bzero(szStr, sizeof (szStr));
    fscanf(pIn, "%d", &iTimeout);
    fscanf(pIn, " %[^\r\n]", szStr);
    FILE* pFile = fopen(szStr, "wb");
    if (NULL == pFile)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    g_oDataSock.SetRecvTimeout(iTimeout);
    while ((iRet = g_oDataSock.Recv(szStr, 4096)) > 0)
    {
        iRet = fwrite(szStr, 1, iRet, pFile);
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
    fprintf(pOut, "= OK | %d\n", iSize);
}

DEFCMDPROC(CmdRecvStrT)
{
    struct sockaddr_in stSockAddr = {0};
    int iRet;
    char szStr[1024];
    int iTimeout;
    bzero(szStr, sizeof (szStr));
    fscanf(pIn, "%d", &iTimeout);
    g_oDataSock.SetRecvTimeout(iTimeout);
    iRet = g_oDataSock.Recv(szStr, 128);
    if (iRet < 0)
    {
        fprintf(pOut, "= Failed | %s\n", strerror(errno));
        return;
    }
    fprintf(pOut, "= OK | %s\n", szStr);
}
/*
DEFCMDPROC(CmdCmdSendLargeFile)
{
    int iRet;
    int iSize = 0;
    char szStr[4096];
    int iPort;
    fscanf(pIn, " %[^\r\n]", szStr);
    
    FILE* pFile = fopen(szStr, "rb");
}

DEFCMDPROC(CmdCommand)
{
    fprintf(pOut, "= OK\n");
    CCmdParser oParser;
    oParser.SetPrompt("Cmd>");
    oParser.RegCmd("quit", CmdQuit);
    oParser.RegCmd("", CmdBadCmd);
    
    oParser.ParseLoop(pIn, pOut);
}

DEFCMDPROC(CmdCommandDaemon)
{
    fprintf(pOut, "= OK\n");
}

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
    CBadgeFile o;
    o.Open(174209756, "/home/thunderliu/test");
    
    o.Close();
    fprintf(pOut, "= OK\n");
}

int main(int argc, char** argv)
{
    g_iArgc = argc;
    g_ppArgv = argv;
    RegCmds();
    g_oParser.RegCmd("test", CmdTest);
    g_oParser.RegCmd("closel", CmdCloseL);
    g_oParser.RegCmd("closed", CmdCloseD);
    g_oParser.RegCmd("bindl", CmdBindL);
    g_oParser.RegCmd("bindd", CmdBindD);
    g_oParser.RegCmd("accept", CmdAccept);
    g_oParser.RegCmd("listen", CmdListen);
    g_oParser.RegCmd("connect", CmdConnect);
    g_oParser.RegCmd("sends", CmdSendStr);
    g_oParser.RegCmd("recvs", CmdRecvStr);
    g_oParser.RegCmd("sendf", CmdSendFile);
    g_oParser.RegCmd("recvft", CmdRecvFileT);
    g_oParser.RegCmd("recvst", CmdRecvStrT);
    
    
    

    //g_oDataSock.Create();
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
            return -1;
        }
        g_oParser.ParseLoop(pFile, stdout, argc, argv);
        fclose(pFile);
        return 0;
    }
    g_oParser.ParseLoop(stdin, stdout, argc, argv);


    return 0;
}
