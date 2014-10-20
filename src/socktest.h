/* 
 * File:   stdinc.h
 * Author: thunderliu
 *
 * Created on 2011年5月26日, 下午3:11
 */

#ifndef STDINC_H
#define	STDINC_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <cstring>
#include <errno.h>

#include <algorithm>
#include <stdint.h>

#ifdef WIN32
#include <winsock.h>
#include <pthread.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/mman.h>
#include <dlfcn.h>
//#include <netpacket/packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
//#include <net/if.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

//#include "MsgBusBody.pb.h"
using namespace std;

#define LOG_POS(fmt, args...) do { fprintf(stderr, "==[ %s() | %s: %d | "fmt" ]==\n", __FUNCTION__, __FILE__, __LINE__, ##args); } while (false)

#define SOCKTEST_VERSION    50

#include "Socket.h"
#include "Section.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "CmdParser.h"

#define DECLARE_REGCMDS extern "C" void RegCmds(CCmdParser* pParser, FILE* pIn, FILE* pOut)
#define BEGIN_REGCMDS extern "C" void RegCmds(CCmdParser* pParser, FILE* pIn, FILE* pOut) {
#define END_REGCMDS }
#define REGCMD(name, info, cmdproc) pParser->RegCmd((name), (info), (cmdproc))
#define P pParser
#define PIN pIn
#define POUT pOut
#define PCMDNAME sCmdName
#define PReadDword(dw) P->ReadDword((dw))
#define PReadQword(ddw) P->ReadQword((ddw))
#define PReadSize(u) P->ReadSize((u))
#define PReadString(str, size) P->ReadString((str), (size))
#define PWriteString(str, args...) \
do \
{ \
    fprintf(pOut, (str), ##args); \
    fflush(pOut); \
} while(false)


#endif	/* STDINC_H */

