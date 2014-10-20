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

#include <unistd.h>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <sys/stat.h>

//#include "MsgBusBody.pb.h"
using namespace std;

#define RETURN_ERR(exit_code) do { fprintf(stderr, "==[RET_ERR: %d %s: %d]==\n", (exit_code), __FILE__, __LINE__); return exit_code; } while (false)

#include "Socket.h"
#include "Section.h"
#include "CmdParser.h"



#endif	/* STDINC_H */

