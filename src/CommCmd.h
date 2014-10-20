/* 
 * File:   CommCmd.h
 * Author: thunderliu
 *
 * Created on 2011年6月9日, 上午10:26
 */

#ifndef COMMCMD_H
#define	COMMCMD_H

#include "CmdParser.h"
#include "socktest.h"


extern FILE* g_pProtoFile;

void RegCmds(CCmdParser* pParser);

DEFCMDPROC(CmdVersion);
DEFCMDPROC(CmdAutoShell);
DEFCMDPROC(CmdQuit);
DEFCMDPROC(CmdBadCmd);
DEFCMDPROC(CmdRun);
DEFCMDPROC(CmdShell);
DEFCMDPROC(CmdImport);
DEFCMDPROC(CmdImportEx);
DEFCMDPROC(CmdMakeProtoWriteProteLine);
DEFCMDPROC(CmdMakeProto);
DEFCMDPROC(CmdMakeBuf);
DEFCMDPROC(CmdParseBuf);
DEFCMDPROC(CmdParseBufEx);
DEFCMDPROC(CmdHelp);

DEFCMDPROC(CmdCloseL);
DEFCMDPROC(CmdCloseD);
DEFCMDPROC(CmdBindLEx);
DEFCMDPROC(CmdBindL);
DEFCMDPROC(CmdBindDEx);
DEFCMDPROC(CmdBindD);
DEFCMDPROC(CmdAccept);
DEFCMDPROC(CmdListen);
DEFCMDPROC(CmdConnect);
DEFCMDPROC(CmdSendStr);
DEFCMDPROC(CmdRecvStr);
DEFCMDPROC(CmdSendFileEx);
DEFCMDPROC(CmdSendFile);
DEFCMDPROC(CmdRecvFile);
DEFCMDPROC(CmdRedirectD);


DEFCMDPROC(CmdCloseU);
DEFCMDPROC(CmdBindUEx);
DEFCMDPROC(CmdBindU);
DEFCMDPROC(CmdSendToStr);
DEFCMDPROC(CmdRecvFromStr);
DEFCMDPROC(CmdSendToFileEx);
DEFCMDPROC(CmdSendToFile);
DEFCMDPROC(CmdRecvFromFile);
DEFCMDPROC(CmdBroadcast);


DEFCMDPROC(CmdCloseC);
DEFCMDPROC(CmdBindCEx);
DEFCMDPROC(CmdBindC);
DEFCMDPROC(CmdPromisc);

DEFCMDPROC(CmdCapUdpFile);
DEFCMDPROC(CmdCapTcpFile);
DEFCMDPROC(CmdCapArpFile);
DEFCMDPROC(CmdSendPackFileEx);
DEFCMDPROC(CmdSendPackFile);

DEFCMDPROC(CmdTime);
DEFCMDPROC(CmdLocalTime);
DEFCMDPROC(CmdError);


DEFCMDPROC(CmdTest);
DEFCMDPROC(CmdTest2);





#endif	/* COMMCMD_H */

