/* 
 * File:   socktest.cpp
 * Author: thunderliu
 *
 * Created on 2011年5月26日, 下午6:38
 */

#include "socktest.h"
#include "CommCmd.h"



char g_szHistory[0x400] = "/dev/null";

void OnExit()
{
    write_history(g_szHistory);
}

void OnInt(int)
{
    printf("\n");
    OnExit();
    _exit(EXIT_FAILURE);
}

int main(int argc, const char** argv)
{
    snprintf(g_szHistory, sizeof(g_szHistory), "%s/.sthistory", getenv("HOME"));
    atexit(OnExit);
    signal(SIGINT, OnInt);
    signal(SIGTERM, OnInt);
    signal(SIGABRT, SIG_IGN);
    //fprintf(stdout, "ignore SIGABRT");
    g_iArgc = argc;
    g_ppArgv = argv;
    CCmdParser oParser;
    RegCmds(&oParser);
    
    using_history();
    stifle_history(100);
    read_history(g_szHistory);
    


    if (argc >= 2)
    {
        FILE* pFile = fopen(argv[1], "r");
        if (pFile == NULL)
        {
            LOG_POS("%s", strerror(errno));
            return -1;
        }
        oParser.RunMode(CCmdParser::RM_BATCH);
        oParser.ParseLoop(pFile, stdout, argc, argv);
        fclose(pFile);
        return 0;
    }
    oParser.ParseLoop(stdin, stdout, argc, argv);


    return 0;
}

