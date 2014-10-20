#include <sys/poll.h>

#include "socktest.h"
#include "CommCmd.h"

#include <sys/ptrace.h>
#include <sys/wait.h>

CTcpSocket g_oLstnSock;
CTcpSocket g_oDataSock;

CUdpSocket g_oUdpSock;

CEthSocket g_oEthSock;


FILE* g_pProtoFile;

void RegCmds(CCmdParser* pParser)
{
    REGCMD("",
           CCommandInfo(""),
           CmdBadCmd);

    REGCMD("version",
           CCommandInfo("show version of socktest.\n"
                        "usage: version\n"),
           CmdVersion);

    REGCMD("autoshell",
           CCommandInfo("unknown command would be run as shell command.\n"
                        "usage: autoshell OPT\n"
                        "OPT = 0: autoshell off\n"
                        "OPT = 1: autoshell on\n"),
           CmdAutoShell);

    REGCMD("quit",
           CCommandInfo("quit current input loop.\n"
                        "usage: quit\n"),
           CmdQuit);

    REGCMD("run",
           CCommandInfo("run socktest script.\n"
                        "usage: run SCRIPT\n"),
           CmdRun);

    REGCMD("shell",
           CCommandInfo("run shell command.\n"
                        "usage: shell COMMAND...\n"),
           CmdShell);

    REGCMD("import",
           CCommandInfo("import library by name.\n"
                        "usage: import LIBRARY\n"),
           CmdImport);

    REGCMD("importx",
           CCommandInfo("import library by full path file name.\n"
                        "usage: importx FILE\n"),
           CmdImportEx);

    REGCMD("makeproto",
           CCommandInfo("make net proto.\n"
                        "usage: makeproto PROTO\n"),
           CmdMakeProto);

    REGCMD("makebuf",
           CCommandInfo("make net packet by proto.\n"
                        "usage: makebuf PACKET PROTO\n"),
           CmdMakeBuf);

    REGCMD("parsebuf",
           CCommandInfo("parse the packet by proto.\n"
                        "usage: parsebuf PACKET PROTO\n"),
           CmdParseBuf);

    REGCMD("parsebufx",
           CCommandInfo("parse the packet by proto.\n"
                        "usage: parsebufx PACKET PROTO OFFSET\n"),
           CmdParseBufEx);

    REGCMD("help",
           CCommandInfo("get the usage of the command.\n"
                        "usage: help COMMAND\n"),
           CmdHelp);

    REGCMD("?",
           CCommandInfo("get the usage of the command.\n"
                        "usage: help COMMAND\n"),
           CmdHelp);

    REGCMD("closel",
           CCommandInfo("close the listen socket.\n"
                        "usage: closel\n"),
           CmdCloseL);

    REGCMD("closed",
           CCommandInfo("close the data stream socket.\n"
                        "usage: closed\n"),
           CmdCloseD);

    REGCMD("bindlx",
           CCommandInfo("bind the listen socket by the address and the port.\n"
                        "usage: bindlx ADDRESS PORT\n"),
           CmdBindLEx);

    REGCMD("bindl",
           CCommandInfo("bind the listen socket by the port.\n"
                        "usage: bindl PORT\n"),
           CmdBindL);

    REGCMD("binddx",
           CCommandInfo("bind the data stream socket by the address and the port.\n"
                        "usage: binddx ADDRESS PORT\n"),
           CmdBindDEx);

    REGCMD("bindd",
           CCommandInfo("bind the data stream socket on any address by the port.\n"
                        "usage: bindd PORT\n"),
           CmdBindD);

    REGCMD("accept",
           CCommandInfo("accept a connection.\n"
                        "usage: accept\n"),
           CmdAccept);

    REGCMD("listen",
           CCommandInfo("begin listening.\n"
                        "usage: listen\n"),
           CmdListen);

    REGCMD("connect",
           CCommandInfo("connect a tcp server by address and port.\n"
                        "usage: connect ADDRESS PORT\n"),
           CmdConnect);

    REGCMD("sends",
           CCommandInfo("send string to the connected tcp server.\n"
                        "usage: sends STRING\n"),
           CmdSendStr);

    REGCMD("recvs",
           CCommandInfo("recieve string from the connected tcp server.\n"
                        "usage: recvs\n"),
           CmdRecvStr);

    REGCMD("sendfx",
           CCommandInfo("send file to the connected tcp server.\n"
                        "usage: sendfx FILE OFFSET\n"),
           CmdSendFileEx);

    REGCMD("sendf",
           CCommandInfo("send file to the connected tcp server.\n"
                        "usage: sendf FILE\n"),
           CmdSendFile);

    REGCMD("recvf",
           CCommandInfo("recieve file from the connected tcp server. need to set timeout option.\n"
                        "usage: recvf FILE TIMEOUT\n"),
           CmdRecvFile);

    REGCMD("redirectd",
           CCommandInfo("redirecting stdin, stdout and stderr to the tcp connection.\n"
                        "usage: redirectd COMMAND...\n"),
           CmdRedirectD);

    REGCMD("closeu",
           CCommandInfo("close the datagram socket.\n"
                        "usage: closeu\n"),
           CmdCloseU);

    REGCMD("bindux",
           CCommandInfo("bind the socket by the address and the port.\n"
                        "usage: bindux ADDRESS PORT\n"),
           CmdBindUEx);

    REGCMD("bindu",
           CCommandInfo("bind the socket on any address by the port.\n"
                        "usage: bindu PORT\n"),
           CmdBindU);

    REGCMD("sendtos",
           CCommandInfo("send string to a remote host.\n"
                        "usage: sendtos REMOTE PORT STRING\n"),
           CmdSendToStr);

    REGCMD("recvfroms",
           CCommandInfo("recieve string from a remote host.\n"
                        "usage: recvfroms\n"),
           CmdRecvFromStr);

    REGCMD("sendtofx",
           CCommandInfo("send file to a remote host.\n"
                        "usage: sendtofx REMOTE PORT FILE OFFSET\n"),
           CmdSendToFileEx);

    REGCMD("sendtof",
           CCommandInfo("send file to a remote host.\n"
                        "usage: sendtof REMOTE PORT FILE\n"),
           CmdSendToFile);

    REGCMD("recvfromf",
           CCommandInfo("recieve file from a remote host. need to set timeout option.\n"
                        "usage: recvfromf FILE TIMEOUT\n"),
           CmdRecvFromFile);

    REGCMD("broadcast",
           CCommandInfo("set the datagram socket broadcast on/off.\n"
                        "usage: broadcast OPT\n"
                        "OPT = 0: broadcast off\n"
                        "OPT = 1: broadcast on\n"),
           CmdBroadcast);

    REGCMD("closec",
           CCommandInfo("close the capture socket.\n"
                        "usage: closec\n"),
           CmdCloseC);

    REGCMD("bindcx",
           CCommandInfo("bind the capture socket on the net interface.\n"
                        "usage: bindcx INTERFACE\n"),
           CmdBindCEx);

    REGCMD("bindc",
           CCommandInfo("just create a capture socket.\n"
                        "usage: bindc\n"),
           CmdBindC);

    REGCMD("promisc",
           CCommandInfo("set the net interface promisc on/off.\n"
                        "usage: promisc INTERFACE OPT\n"
                        "OPT = 0: promisc off\n"
                        "OPT = 1: promisc on\n"),
           CmdPromisc);

    REGCMD("capudpf",
           CCommandInfo("capture udp packet file. 0.0.0.0 can be set to ADDR, and 0 can be set to PORT.\n"
                        "usage: capudpf FILE TIMEOUT SADDR SPORT DADDR DPORT\n"),
           CmdCapUdpFile);

    REGCMD("captcpf",
           CCommandInfo("capture tcp packet file. 0.0.0.0 can be set to ADDR, and 0 can be set to PORT.\n"
                        "usage: captcpf FILE TIMEOUT SADDR SPORT DADDR DPORT\n"),
           CmdCapTcpFile);

    REGCMD("caparpf",
           CCommandInfo("capture arp packet file.\n"
                        "usage: caparpf FILE TIMEOUT\n"),
           CmdCapArpFile);

    REGCMD("sendpktfx",
           CCommandInfo("send packet file to the net interface.\n"
                        "usage: sendpktfx FILE OFFSET\n"),
           CmdSendPackFileEx);

    REGCMD("sendpktf",
           CCommandInfo("send packet file to the net interface.\n"
                        "usage: sendpktf FILE\n"),
           CmdSendPackFile);

    REGCMD("time",
           CCommandInfo("get timestamp.\n"
                        "usage: time\n"),
           CmdTime);

    REGCMD("localtime",
           CCommandInfo("get local time by timestamp.\n"
                        "usage: localtime TIME\n"),
           CmdLocalTime);
    
    REGCMD("error",
           CCommandInfo("get a string describing the error code passed in the argument.\n"
                        "usage: error ERRCODE\n"),
           CmdError);


    REGCMD("test",
           CCommandInfo("test command. \n"
                        "usage: test\n"),
           CmdTest);

    REGCMD("test2",
           CCommandInfo("test2 command. \n"
                        "usage: test2\n"),
           CmdTest2);

}

DEFCMDPROC(CmdVersion)
{
    PWriteString("socktest r%u\nhttp://hi.baidu.com/ligh0721\n", SOCKTEST_VERSION);
}

DEFCMDPROC(CmdAutoShell)
{
    uint32_t dwOpt = 0;
    PReadDword(dwOpt);
    switch (dwOpt)
    {
    case 0:
    case 1:
        P->AutoShell(dwOpt != 0);
        break;
    default:
        PWriteString("= Failed | unknown opt(%d)\n", dwOpt);
        return -1;
    }

    return 0;
}

DEFCMDPROC(CmdQuit)
{
    //PWriteString("= OK\n");
    P->Quit();
    return 0;
}

DEFCMDPROC(CmdBadCmd)
{
    if (P->GetStringAddr()[0] == '#')
    {
        return 0;
    }

    PWriteString("= OK | Run Shell:\n");
    int iRet;
    iRet = system(P->GetStringAddr());
    if (iRet < 0)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    ////PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdRun)
{
    char szStr[1024];
    PReadString(szStr, sizeof (szStr));
    FILE* pFile = fopen64(szStr, "r");
    if (pFile == NULL)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK\n");

    CCmdParser::RUNMODE eOldRunMode = P->RunMode();
    P->RunMode(CCmdParser::RM_BATCH);
    int iArgc;
    char* ppArgv[256];
    strncpy(ppArgv[0] = new char[1024], pParser->GetArgv()[0], 1024);
    strncpy(ppArgv[1] = new char[1024], szStr, 1024);
    for (iArgc = 2; P->ReadStringEnd(ppArgv[iArgc] = new char[1024], 1024) >= 0; iArgc++);

    pParser->ParseLoop(pFile, pOut, iArgc, const_cast<const char**>(ppArgv));
    pParser->Continue();
    fclose(pFile);
    for (int i = 0; i < iArgc; i++)
    {
        delete[] ppArgv[i];
    }
    P->RunMode(eOldRunMode);

    return 0;
}

DEFCMDPROC(CmdShell)
{
    int iRet;
    iRet = system(P->GetPos());
    if (iRet < 0)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    ////PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdImportEx)
{
    char szStr[1024];
    PReadString(szStr, sizeof (szStr));
    void* pDl = dlopen(szStr, RTLD_LAZY);
    if (!pDl)
    {
        PWriteString("= Failed | %s\n", dlerror());
        return -1;

    }
    REGCMDSPROC RegCmds = (REGCMDSPROC)dlsym(pDl, "RegCmds");
    if (!RegCmds)
    {
        PWriteString("= Failed | %s\n", dlerror());
        return -1;

    }
    RegCmds(pParser, pIn, pOut);
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdImport)
{
    char szStr[1024];
    char szLibStr[1024];
    PReadString(szStr, sizeof (szStr));
    snprintf(szLibStr, sizeof (szLibStr), "/usr/local/lib/socktest/%s.slb", szStr);
    void* pDl = dlopen(szLibStr, RTLD_LAZY);
    if (!pDl)
    {
        PWriteString("= Failed | %s\n", dlerror());
        return -1;
    }
    REGCMDSPROC RegCmds = (REGCMDSPROC)dlsym(pDl, "RegCmds");
    if (!RegCmds)
    {
        PWriteString("= Failed | %s\n", dlerror());
        return -1;
    }
    RegCmds(pParser, pIn, pOut);
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdMakeProtoWriteProteLine)
{
    fprintf(g_pProtoFile, "%s\n", pParser->GetStringAddr());
}

DEFCMDPROC(CmdMakeProto)
{
    CIoReadline* pOldInst = CIoReadline::Instance();
    CCmdParser oParser;
    oParser.SetPrompt("proto> ");
    char szStr[1024];
    PReadString(szStr, sizeof (szStr));
    g_pProtoFile = fopen64(szStr, "w");
    if (g_pProtoFile == NULL)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        fclose(g_pProtoFile);
        return -1;
    }
    //PWriteString("= OK\n");
    oParser.RegCmd("quit",
                   CCommandInfo("quit makeproto.\n"
                                "usage: quit\n"),
                   CmdQuit);
    oParser.RegCmd("",
                   CCommandInfo(""),
                   CmdMakeProtoWriteProteLine);
    oParser.ParseLoop(pIn, pOut, pParser->GetArgc(), pParser->GetArgv());

    CIoReadline::Instance(pOldInst);

    fclose(g_pProtoFile);
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdMakeBuf)
{
    char szStr[1024];
    uint8_t acBuf[40960];
    int iSize = 0;
    PReadString(szStr, sizeof (szStr));
    FILE* pBufFile = fopen64(szStr, "w");
    int iRet = 0;
    if (pBufFile == NULL)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }

    PReadString(szStr, sizeof (szStr));
    FILE* pProtoFile = fopen64(szStr, "r");
    if (pProtoFile == NULL)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        fclose(pBufFile);
        return -1;
    }

    SYMBOLTABLE oGlbSym;
    CSectionStruct* pSec;
    CSectionStruct* pLastSec;
    while (!feof(pProtoFile))
    {
        if (!fscanf(pProtoFile, "%[^\r\n]", szStr)) //\n
        {
            break;
        }
        fscanfln(pProtoFile);
        if (*(uint16_t*)"st" == *(uint16_t*)szStr)
        {
            pSec = CSectionStruct::CreateSectionStructFromLine(oGlbSym, szStr);
            if (pSec)
            {
                pLastSec = pSec;
            }
            else
            {
                // error
                PWriteString("= Failed | create struct err(%s)\n", szStr);
                fclose(pProtoFile);
                fclose(pBufFile);
                return -1;
            }
        }
        else if ('@' == *(uint8_t*)szStr)
        {
            pLastSec->AddAction(&szStr[1]);
        }
        else
        {
            // error
            PWriteString("= Failed | (%s) not support\n", szStr);
            fclose(pProtoFile);
            fclose(pBufFile);
            return -1;
        }

    }
    /////
    //    printf("%s's SymbolTable:\n", pLastSec->m_sName.c_str());
    //    for (SYMBOLTABLE::iterator it = oSym.begin(); it != oSym.end(); it++)
    //    {
    //        printf("%s\n", it->first.c_str());
    //    }
    /////
    fclose(pProtoFile);
    SYMBOLVALUETABLE oGlbSymVal;
    iSize = pLastSec->Serialize(acBuf, pIn, pOut, oGlbSym, oGlbSymVal, true, pParser);

    while (iSize > 0)
    {
        iRet = fwrite(acBuf + iRet, 1, iSize < 4096 ? iSize : 4096, pBufFile);
        if (iRet <= 0)
        {
            // error
            PWriteString("= Failed | %s\n", strerror(errno));
            fclose(pBufFile);
            return -1;
        }
        iSize -= iRet;
    }
    fclose(pBufFile);
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdParseBuf)
{
    strcat(const_cast<char*>(pParser->GetStringAddr()), " 0");
    return CmdParseBufEx(pParser, pIn, pOut, sCmdName);
}

DEFCMDPROC(CmdParseBufEx)
{
    char szStr[1024];
    //uint8_t acBuf[40960];
    size_t uSize = 0;
    size_t uPos = 0;
    PReadString(szStr, sizeof (szStr));
    /*
    FILE* pBufFile = fopen64(szStr, "r");
    int iRet = 0;
    if (pBufFile == NULL)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return;
    }
     */
    CFileMemMap oMm;
    if (!oMm.Create(szStr, true))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }

    PReadString(szStr, sizeof (szStr));
    FILE* pProtoFile = fopen64(szStr, "r");
    if (pProtoFile == NULL)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }

    PReadDword((uint32_t&)uPos);

    SYMBOLTABLE oGlbSym;
    CSectionStruct* pSec;
    CSectionStruct* pLastSec;
    while (!feof(pProtoFile))
    {
        if (!fscanf(pProtoFile, "%[^\r\n]", szStr))//\n
        {
            break;
        }
        fscanfln(pProtoFile);
        if (*(uint16_t*)"st" == *(uint16_t*)szStr)
        {
            pSec = CSectionStruct::CreateSectionStructFromLine(oGlbSym, szStr);
            if (pSec)
            {
                pLastSec = pSec;
            }
            else
            {
                // error
                break;
            }
        }
        else if ('@' == *(uint8_t*)szStr)
        {
            pLastSec->AddAction(&szStr[1]);
        }
        else
        {
            // error
            fclose(pProtoFile);
            PWriteString("= Failed | bad line(%s)\n", szStr);
            return -1;
        }

    }
    fclose(pProtoFile);
    /*
    while (!feof(pBufFile))
    {
        iRet = fread(acBuf + iSize, 1, 4096, pBufFile);
        if (iRet <= 0)
        {
            // error
            PWriteString("= Failed | %s\n", strerror(errno));
            break;
        }
        iSize += iRet;
    }
    fclose(pBufFile);
     */
    SYMBOLVALUETABLE oGlbSymVal;
    PWriteString("%s:\n", pLastSec->m_sName.c_str());
    uSize = pLastSec->Serialize(oMm, uPos, pIn, pOut, "", "", oGlbSym, oGlbSymVal, false, pParser);
    PWriteString("0x%08X\n", (uint32_t)uSize);
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdHelp)
{
    char szStr[1024];
    PReadString(szStr, sizeof (szStr));
    const char* pStr = pParser->GetCommandInfo(szStr).c_str();
    if (strlen(pStr))
    {
        PWriteString("%s\n", pStr);
    }
    else
    {
        PWriteString("command \"%s\" was not found.\n", szStr);
    }

    return 0;
}

DEFCMDPROC(CmdCloseL)
{
    if (!g_oLstnSock.Close())
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdCloseD)
{
    if (!g_oDataSock.Close())
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdBindLEx)
{
    char szStr[1024];
    int iPort;
    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iPort);
    if (!g_oLstnSock.Bind(szStr, iPort, true))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %u\n", iPort);
    return 0;
}

DEFCMDPROC(CmdBindL)
{
    int iPort;
    PReadDword((uint32_t&)iPort);
    if (!g_oLstnSock.Bind(NULL, iPort, true))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %u\n", iPort);
    return 0;
}

DEFCMDPROC(CmdBindDEx)
{
    char szStr[1024];
    int iPort;
    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iPort);
    if (!g_oDataSock.Bind(szStr, iPort, true))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iPort);
    return 0;
}

DEFCMDPROC(CmdBindD)
{
    int iPort;
    PReadDword((uint32_t&)iPort);
    if (!g_oDataSock.Bind(NULL, iPort, true))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iPort);
    return 0;
}

DEFCMDPROC(CmdAccept)
{
    if (!g_oLstnSock.Accept(&g_oDataSock))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %s:%d\n", ((CSockAddrIn*)g_oDataSock.GetSockAddr())->GetAddr(), ((CSockAddrIn*)g_oDataSock.GetSockAddr())->GetPort());
    return 0;
}

DEFCMDPROC(CmdListen)
{
    if (!g_oLstnSock.Listen(5))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdConnect)
{
    char szStr[0x1000];
    uint32_t dwPort;
    PReadString(szStr, sizeof (szStr));
    PReadDword(dwPort);
    if (!g_oDataSock.Connect(szStr, dwPort))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdSendStr)
{
    int iRet;
    char szStr[10240];
    PReadString(szStr, sizeof (szStr));
    iRet = g_oDataSock.Send(szStr, strlen(szStr));
    if (iRet <= 0)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iRet);
    return 0;
}

DEFCMDPROC(CmdRecvStr)
{
    int iRet;
    char szStr[1024];
    bzero(szStr, sizeof (szStr));
    g_oDataSock.SetRecvTimeout(0);
    iRet = g_oDataSock.Recv(szStr, 128);
    if (iRet <= 0)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %s\n", szStr);
    return 0;
}

DEFCMDPROC(CmdSendFileEx)
{
    int iRet;
    int iSize = 0;
    char szStr[0x1000];
    PReadString(szStr, sizeof (szStr));
    FILE* pFile = fopen64(szStr, "rb");
    if (NULL == pFile)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }

    uint32_t dwOffset = 0;
    PReadDword(dwOffset);
    fseeko64(pFile, dwOffset, SEEK_SET);

    while (!feof(pFile))
    {
        iRet = fread(szStr, 1, 0x1000, pFile);
        if (iRet < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            break;
        }
        iSize += iRet;
        iRet = g_oDataSock.Send(szStr, iRet);
        if (iRet < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            break;
        }
    }
    fclose(pFile);
    if (iRet < 0 && !iSize)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iSize);
    return 0;
}

DEFCMDPROC(CmdSendFile)
{
    strcat(const_cast<char*>(pParser->GetStringAddr()), " 0");
    return CmdSendFileEx(pParser, pIn, pOut, sCmdName);
}

DEFCMDPROC(CmdRecvFile)
{
    int iRet;
    int iSize = 0;
    int iTimeout;
    char szStr[0x1000];
    //bzero(szStr, sizeof (szStr));
    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iTimeout);
    FILE* pFile = fopen64(szStr, "wb");
    if (NULL == pFile)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    g_oDataSock.SetRecvTimeout(0);
    iRet = g_oDataSock.Recv(szStr, 0x1000);
    g_oDataSock.SetRecvTimeout(iTimeout);
    while (iRet > 0)
    {
        iRet = fwrite(szStr, 1, iRet, pFile);
        if (iRet < 0)
        {
            break;
        }
        iSize += iRet;
        iRet = g_oDataSock.Recv(szStr, 0x1000);
    }
    fclose(pFile);
    if (iRet < 0 && !iSize)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iSize);
    return 0;
}

DEFCMDPROC(CmdRedirectD)
{
    char szCmd[0x100];
    char szStr[0x1000];
    PReadString(szCmd, sizeof (szCmd));
    snprintf(szStr, sizeof (szStr), "%s %s", szCmd, P->GetPos());

    pid_t iPid = fork();
    switch (iPid)
    {
    case -1:
        PWriteString("= Failed | %s\n", strerror(errno));
        exit(EXIT_FAILURE);
        break;

    case 0:
    {
        int iOrgIn = dup(STDIN_FILENO);
        int iOrgOut = dup(STDOUT_FILENO);
        int iOrgErr = dup(STDERR_FILENO);
        if (dup2(g_oDataSock, STDIN_FILENO) < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (dup2(g_oDataSock, STDOUT_FILENO) < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (dup2(g_oDataSock, STDERR_FILENO) < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        int iRet = execlp(szCmd, szStr, (char*)NULL);

        dup2(iOrgIn, STDIN_FILENO);
        dup2(iOrgOut, STDOUT_FILENO);
        dup2(iOrgErr, STDERR_FILENO);
        close(iOrgIn);
        close(iOrgOut);
        close(iOrgErr);

        if (iRet < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_SUCCESS);
    }
    
    default:
        waitpid(iPid, NULL, 0);
    }


    ////PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdCloseU)
{
    if (!g_oUdpSock.Close())
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdBindUEx)
{
    char szStr[1024];
    int iPort;
    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iPort);
    if (!g_oUdpSock.Bind(szStr, iPort, true))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iPort);
    return 0;
}

DEFCMDPROC(CmdBindU)
{
    int iPort;
    PReadDword((uint32_t&)iPort);
    if (!g_oUdpSock.Bind(NULL, iPort))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iPort);
    return 0;
}

DEFCMDPROC(CmdSendToStr)
{
    int iRet;
    char szStr[1024];
    int iPort;
    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iPort);
    CSockAddrIn oSockAddr(szStr, iPort);
    PReadString(szStr, sizeof (szStr));
    if (szStr[0] == '\"' && szStr[strlen(szStr) - 1] == '\"')
    {
        char szStr2[1024] = {};
        memmove(szStr2, &szStr[1], strlen(szStr) - 2);
        iRet = g_oUdpSock.SendTo(szStr2, strlen(szStr2), &oSockAddr);
    }
    else
    {
        iRet = g_oUdpSock.SendTo(szStr, strlen(szStr), &oSockAddr);
    }
    
    if (-1 == iRet)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iRet);
    return 0;
}

DEFCMDPROC(CmdRecvFromStr)
{
    int iRet;
    char szStr[1024];
    bzero(szStr, sizeof (szStr));
    CSockAddrIn oSockAddr;
    g_oUdpSock.SetRecvTimeout(0);
    iRet = g_oUdpSock.RecvFrom(szStr, sizeof (szStr) - 1, &oSockAddr);
    if (iRet <= 0)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %s:%d %s\n", oSockAddr.GetAddr(), oSockAddr.GetPort(), szStr);
    return 0;
}

DEFCMDPROC(CmdSendToFileEx)
{
    int iRet;
    int iSize = 0;
    char szStr[4096];
    int iPort;
    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iPort);
    CSockAddrIn oSockAddr(szStr, iPort);
    PReadString(szStr, sizeof (szStr));
    FILE* pFile = fopen64(szStr, "rb");
    if (NULL == pFile)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }

    uint32_t dwOffset = 0;
    PReadDword(dwOffset);
    fseeko64(pFile, dwOffset, SEEK_SET);

    while (!feof(pFile))
    {
        iRet = fread(szStr, 1, 4096, pFile);
        if (iRet < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            break;
        }
        iSize += iRet;
        iRet = g_oUdpSock.SendTo(szStr, iRet, &oSockAddr);
        if (iRet < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            break;
        }
    }
    fclose(pFile);
    if (iRet < 0)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iSize);
    return 0;
}

DEFCMDPROC(CmdSendToFile)
{
    strcat(const_cast<char*>(pParser->GetStringAddr()), " 0");
    return CmdSendToFileEx(pParser, pIn, pOut, sCmdName);
}

DEFCMDPROC(CmdRecvFromFile)
{
    int iRet;
    int iSize = 0;
    int iTimeout;
    char szStr[4096];
    //bzero(sStr, sizeof (sStr));
    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iTimeout);
    FILE* pFile = fopen64(szStr, "wb");
    if (NULL == pFile)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    CSockAddrIn oSockAddr;
    g_oUdpSock.SetRecvTimeout(0);
    iRet = g_oUdpSock.RecvFrom(szStr, 4096, &oSockAddr);
    g_oUdpSock.SetRecvTimeout(iTimeout);
    while (iRet > 0)
    {

        iRet = fwrite(szStr, 1, iRet, pFile);
        if (iRet < 0)
        {
            break;
        }
        iSize += iRet;
        iRet = g_oUdpSock.RecvFrom(szStr, 4096, &oSockAddr);
    }
    fclose(pFile);
    if (iRet < 0 && !iSize)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %s:%d %d\n", oSockAddr.GetAddr(), oSockAddr.GetPort(), iSize);
    return 0;
}

DEFCMDPROC(CmdBroadcast)
{
    char szStr[1024];
    uint32_t dwOpt = 0;
    PReadDword(dwOpt);
    switch (dwOpt)
    {
    case 0:
    case 1:
        if (!g_oUdpSock.SetSockOpt(SOL_SOCKET, SO_BROADCAST, &dwOpt, sizeof (dwOpt)))
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            return -1;
        }
        break;
    default:
        PWriteString("= Failed | unknown opt(%d)\n", dwOpt);
        return -1;
    }

    return 0;
}

DEFCMDPROC(CmdCloseC)
{
    if (!g_oEthSock.Close())
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdBindCEx)
{
    char szStr[1024];
    int iPort;
    PReadString(szStr, sizeof (szStr));
    if (!g_oEthSock.Bind(szStr))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        //return -1;
    }
    //PWriteString("= OK | %d\n", iPort);
    return 0;
}

DEFCMDPROC(CmdBindC)
{
    g_oEthSock.Bind(NULL);

    return 0;
}

DEFCMDPROC(CmdPromisc)
{
    char szStr[1024];
    PReadString(szStr, sizeof (szStr));
    uint32_t dwOpt = 0;
    PReadDword(dwOpt);
    switch (dwOpt)
    {
    case 0:
    case 1:
        if (!g_oEthSock.SetPromisc(szStr, dwOpt != 0))
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            return -1;
        }
        break;
    default:
        PWriteString("= Failed | unknown opt(%d)\n", dwOpt);
        return -1;
    }

    return 0;
}

DEFCMDPROC(CmdCapUdpFile)
{
    int iRet;
    int iSize = 0;
    int iTimeout;
    char szStr[4096];
    uint32_t dwSAddr;
    string sSAddr;
    int iSPort;
    uint32_t dwDAddr;
    string sDAddr;
    int iDPort;

    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iTimeout);

    struct ethhdr* pEthHdr;
    struct iphdr* pIpHdr;
    struct udphdr* pUdpHdr;
    char* pUdpData;
    FILE* pFile = fopen64(szStr, "wb");
    if (NULL == pFile)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PReadString(szStr, sizeof (szStr));
    dwSAddr = inet_addr(szStr);
    PReadDword((uint32_t&)iSPort);

    PReadString(szStr, sizeof (szStr));
    dwDAddr = inet_addr(szStr);
    PReadDword((uint32_t&)iDPort);

    g_oEthSock.SetRecvTimeout(0);
    iRet = g_oEthSock.Recv(szStr, sizeof (szStr));

    while (iRet > 0)
    {
        pEthHdr = (struct ethhdr*)szStr;
        if (ntohs(pEthHdr->h_proto) == ETH_P_IP)
        {
            pIpHdr = (struct iphdr*)(pEthHdr + 1);
            if (pIpHdr->protocol == IPPROTO_UDP)
            {
                pUdpHdr = (struct udphdr*)(((char*)pIpHdr) + pIpHdr->ihl * 4);
                sSAddr = inet_ntoa(*(struct in_addr*)&pIpHdr->saddr);
                sDAddr = inet_ntoa(*(struct in_addr*)&pIpHdr->daddr);

                if (!((dwSAddr ^ pIpHdr->saddr) & dwSAddr) && !(((uint16_t)iSPort ^ ntohs(pUdpHdr->source)) & (uint16_t)iSPort) && !((dwDAddr ^ pIpHdr->daddr) & dwDAddr) && !(((uint16_t)iDPort ^ ntohs(pUdpHdr->dest)) & (uint16_t)iDPort))
                {
                    uint16_t wDataLen = ntohs(pUdpHdr->len) - sizeof (struct udphdr);
                    PWriteString("%u | %s:%d -> %s:%d\n", wDataLen, sSAddr.c_str(), ntohs(pUdpHdr->source), sDAddr.c_str(), ntohs(pUdpHdr->dest));
                    pUdpData = (char*)(pUdpHdr + 1);
                    iRet = fwrite(pUdpData, 1, wDataLen, pFile);
                    if (iRet < 0)
                    {
                        break;
                    }
                    fflush(pFile);
                    iSize += iRet;
                    g_oEthSock.SetRecvTimeout(iTimeout);
                }
            }
        }
        iRet = g_oEthSock.Recv(szStr, sizeof (szStr));
    }
    fclose(pFile);
    if (iRet < 0 && !iSize)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK | %s:%d %d\n", oSockAddr.GetAddr(), oSockAddr.GetPort(), iSize);
    return 0;
}

DEFCMDPROC(CmdCapTcpFile)
{
    int iRet;
    int iSize = 0;
    int iTimeout;
    char szStr[4096];
    uint32_t dwSAddr;
    string sSAddr;
    int iSPort;
    uint32_t dwDAddr;
    string sDAddr;
    int iDPort;

    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iTimeout);

    struct ethhdr* pEthHdr;
    struct iphdr* pIpHdr;
    struct tcphdr* pTcpHdr;
    char* pTcpData;
    FILE* pFile = fopen64(szStr, "wb");
    if (NULL == pFile)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PReadString(szStr, sizeof (szStr));
    dwSAddr = inet_addr(szStr);
    PReadDword((uint32_t&)iSPort);

    PReadString(szStr, sizeof (szStr));
    dwDAddr = inet_addr(szStr);
    PReadDword((uint32_t&)iDPort);

    g_oEthSock.SetRecvTimeout(0);
    iRet = g_oEthSock.Recv(szStr, sizeof (szStr));

    while (iRet > 0)
    {
        pEthHdr = (struct ethhdr*)szStr;
        if (ntohs(pEthHdr->h_proto) == ETH_P_IP)
        {
            pIpHdr = (struct iphdr*)(pEthHdr + 1);
            if (pIpHdr->protocol == IPPROTO_TCP)
            {
                pTcpHdr = (struct tcphdr*)(((char*)pIpHdr) + pIpHdr->ihl * 4);
                sSAddr = inet_ntoa(*(struct in_addr*)&pIpHdr->saddr);
                sDAddr = inet_ntoa(*(struct in_addr*)&pIpHdr->daddr);

                if (!((dwSAddr ^ pIpHdr->saddr) & dwSAddr) && !(((uint16_t)iSPort ^ ntohs(pTcpHdr->source)) & (uint16_t)iSPort) && !((dwDAddr ^ pIpHdr->daddr) & dwDAddr) && !(((uint16_t)iDPort ^ ntohs(pTcpHdr->dest)) & (uint16_t)iDPort))
                {
                    uint16_t wDataLen = ntohs(pIpHdr->tot_len) - (pIpHdr->ihl + pTcpHdr->doff) * 4;
                    PWriteString("%u | %s:%d -> %s:%d\n", wDataLen, sSAddr.c_str(), ntohs(pTcpHdr->source), sDAddr.c_str(), ntohs(pTcpHdr->dest));
                    pTcpData = (char*)(pTcpHdr + 1);
                    iRet = fwrite(pTcpData, 1, wDataLen, pFile);
                    if (iRet < 0)
                    {
                        break;
                    }
                    fflush(pFile);
                    iSize += iRet;
                    g_oEthSock.SetRecvTimeout(iTimeout);
                }
            }
        }
        iRet = g_oEthSock.Recv(szStr, sizeof (szStr));
    }
    fclose(pFile);
    if (iRet < 0 && !iSize)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK | %s:%d %d\n", oSockAddr.GetAddr(), oSockAddr.GetPort(), iSize);
    return 0;
}

DEFCMDPROC(CmdCapArpFile)
{
    int iRet;
    int iSize = 0;
    int iTimeout;
    char szStr[4096];
    uint32_t dwSAddr;
    string sSAddr;
    int iSPort;
    uint32_t dwDAddr;
    string sDAddr;
    int iDPort;

    PReadString(szStr, sizeof (szStr));
    PReadDword((uint32_t&)iTimeout);

    struct ethhdr* pEthHdr;
    struct etharphdr* pArpHdr;
    //struct arpreq* pArpReq;

    FILE* pFile = fopen64(szStr, "wb");
    if (NULL == pFile)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PReadString(szStr, sizeof (szStr));
    //dwSAddr = inet_addr(szStr);
    //PReadDword((uint32_t&)iSPort);

    //PReadString(szStr, sizeof (szStr));
    //dwDAddr = inet_addr(szStr);
    //PReadDword((uint32_t&)iDPort);

    g_oEthSock.SetRecvTimeout(0);
    iRet = g_oEthSock.Recv(szStr, sizeof (szStr));
    g_oEthSock.SetRecvTimeout(iTimeout);

    while (iRet > 0)
    {
        pEthHdr = (struct ethhdr*)szStr;
        if (ntohs(pEthHdr->h_proto) == ETH_P_ARP)
        {
            pArpHdr = (struct etharphdr*)(pEthHdr + 1);
            if (ntohs(pArpHdr->ar_hdr.ar_hrd) == ARPHRD_ETHER && ntohs(pArpHdr->ar_hdr.ar_pro) == ETH_P_IP)
            {
                PWriteString("op:%d |", ntohs(pArpHdr->ar_hdr.ar_op));
                //pArpReq = (struct arpreq*)(pArpHdr + 1);
                sSAddr = inet_ntoa(*(struct in_addr*)pArpHdr->ar_sip);
                sDAddr = inet_ntoa(*(struct in_addr*)pArpHdr->ar_tip);
                PWriteString("%02x:%02x:%02x:%02x:%02x:%02x (%s) -> %02x:%02x:%02x:%02x:%02x:%02x (%s)\n", pArpHdr->ar_sha[0], pArpHdr->ar_sha[1], pArpHdr->ar_sha[2], pArpHdr->ar_sha[3], pArpHdr->ar_sha[4], pArpHdr->ar_sha[5], sSAddr.c_str(), pArpHdr->ar_tha[0], pArpHdr->ar_tha[1], pArpHdr->ar_tha[2], pArpHdr->ar_tha[3], pArpHdr->ar_tha[4], pArpHdr->ar_tha[5], sDAddr.c_str());
                //PWriteString("", pArpReq->arp_pa.)
            }
        }
        iRet = g_oEthSock.Recv(szStr, sizeof (szStr));
    }
    fclose(pFile);
    if (iRet < 0 && !iSize)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    //PWriteString("= OK | %s:%d %d\n", oSockAddr.GetAddr(), oSockAddr.GetPort(), iSize);
    return 0;
}

DEFCMDPROC(CmdSendPackFileEx)
{
    int iRet;
    int iSize = 0;
    char szStr[0x1000];
    PReadString(szStr, sizeof (szStr));
    FILE* pFile = fopen64(szStr, "rb");
    if (NULL == pFile)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }

    uint32_t dwOffset = 0;
    PReadDword(dwOffset);
    fseeko64(pFile, dwOffset, SEEK_SET);

    while (!feof(pFile))
    {
        iRet = fread(szStr, 1, 0x1000, pFile);
        if (iRet < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            break;
        }
        iSize += iRet;
        iRet = g_oEthSock.Send(szStr, iRet);
        if (iRet < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            break;
        }
    }
    fclose(pFile);
    if (iRet < 0 && !iSize)
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }
    PWriteString("= OK | %d\n", iSize);
    return 0;
}

DEFCMDPROC(CmdSendPackFile)
{
    strcat(const_cast<char*>(pParser->GetStringAddr()), " 0");
    return CmdSendPackFileEx(pParser, pIn, pOut, sCmdName);
}

DEFCMDPROC(CmdTime)
{
    PWriteString("= OK | %u\n", (uint32_t)time(NULL));
}

DEFCMDPROC(CmdLocalTime)
{
    time_t uTime;
    PReadSize((size_t&)uTime);
    struct tm stTm = {};
    localtime_r(&uTime, &stTm);
    PWriteString("= OK | %04u-%02u-%02u %02u:%02u:%02u\n", stTm.tm_year + 1900, stTm.tm_mon + 1, stTm.tm_mday, stTm.tm_hour, stTm.tm_min, stTm.tm_sec);
}

DEFCMDPROC(CmdError)
{
    int iErr;
    PReadDword((uint32_t&)iErr);
    PWriteString("= OK | %s\n", strerror(iErr));
}

typedef struct
{
    char szAddr[128];
    uint32_t dwPort;
    int iTimeOut;
} S_TP;

S_TP g_stP;

void* WriteThread(void*)
{
    uint8_t acReq[] = {0x02, 0x00, 0x2f, 0x00, 0x00, 0x01, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x62, 0x3a, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x62, 0x3a, 0xdc, 0x0a, 0x62, 0x3a, 0xdc, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
    uint8_t acRsp[2048];

    CTcpSocket* pTcp = new CTcpSocket[1];
    uint32_t dwSucc = 0;
    uint32_t dwFailed = 0;
    int iRet;
    for (uint32_t i = 0; i < 1; i++)
    {
        pTcp[i].Bind(NULL, 0);
        if (pTcp[i].Connect(g_stP.szAddr, (int)g_stP.dwPort))
        {
            pTcp[i].Send(acReq, sizeof (acReq));
            pTcp[i].SetRecvTimeout(g_stP.iTimeOut);
            iRet = pTcp[i].Recv(acRsp, sizeof (acRsp));
            if (iRet < 0)
            {
                dwFailed++;
                printf("ERR | %s\n", strerror(errno));
            }
            else
            {
                if (iRet == 753)
                {
                    dwSucc++;
                    printf("Succ Count: %u\n", dwSucc);

                }
                else
                {
                    printf("%u\n", iRet);
                    printf("ERR\n");
                }

            }


        }
        else
        {
            dwFailed++;
            //PWriteString("Conn Failed Count: %u\n", dwSucc);
            printf("ERR\n");
        }
    }
    //PWriteString("Conn Result: %u/%u\n", dwSucc, dwConnCount);

    /*
    for (uint32_t i = 0; i < dwConnCount; i++)
    {
        pTcp[i].Close();
    }
     */
    delete[] pTcp;
}

DEFCMDPROC(CmdTest)
{
    PWriteString("%d\n", 0xfffff050);

    return 0;
}

DEFCMDPROC(CmdTest2)
{

}

