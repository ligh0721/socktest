#include <socktest/socktest.h>

DECLARE_REGCMDS;

typedef struct
{
    FILE* pOut;
    uint32_t dwStartI;
    uint32_t dwEndI;
    char* pFileName;
    size_t* pOffset;
    size_t* pSubSize;

    char* pStart;
} S_THREAD_PARAM;

void* WriteThread(S_THREAD_PARAM* pParam)
{
    char szStr[1024];
    const size_t BUFF_SIZE = 0x1000;
    //uint8_t szBuff[BUFF_SIZE];
    FILE* pFile;
    size_t uToWrite;
    char* pOff;

    for (uint32_t i = pParam->dwStartI; i < pParam->dwEndI; i++)
    {
        snprintf(szStr, sizeof (szStr), "%s.%d", pParam->pFileName, i);
        pFile = fopen(szStr, "w");
        pOff = pParam->pStart + pParam->pOffset[i];
        //auSubSize[i] = auOffset[i + 1] - auOffset[i];
        fprintf(stdout, "%u: [%lu - %lu) %lu\n", i, pParam->pOffset[i], pParam->pOffset[i + 1], pParam->pSubSize[i]);

        while (pParam->pSubSize[i])
        {
            uToWrite = BUFF_SIZE > pParam->pSubSize[i] ? pParam->pSubSize[i] : BUFF_SIZE;
            uToWrite = fwrite(pOff, uToWrite, 1, pFile) * uToWrite;
            if (uToWrite <= 0)
            {
                fprintf(pParam->pOut, "= Failed | %s\n", strerror(errno));
                return NULL;
            }
            pParam->pSubSize[i] -= uToWrite;
            pOff += uToWrite;
        }

        fclose(pFile);
    }
    return NULL;
}

DEFCMDPROC(CmdAscFileSplitEx)
{
    char szFileName[1024];
    char szStr[1024];
    uint32_t dwSubCount;
    uint32_t dwThreadCount;
    PReadString(szFileName, sizeof (szFileName));
    PReadDword(dwSubCount);
    PReadDword(dwThreadCount);
    CFileMemMap oFm;
    if (!oFm.Create(szFileName, true))
    {
        PWriteString("= Failed | %s\n", strerror(errno));
        return -1;
    }

    size_t uFileSize = oFm.GetSize();

    printf("FileSize: %lu\n", uFileSize);
    size_t* auOffset = new size_t[dwSubCount + 1];
    size_t* auSubSize = new size_t[dwSubCount];
    size_t uSubSize = uFileSize / dwSubCount;

    auOffset[0] = 0;
    uint32_t i;
    for (i = 1; i < dwSubCount; i++)
    {
        auOffset[i] = auOffset[i - 1] + uSubSize;
    }
    auOffset[i] = uFileSize;

    char* pStart = (char*)((void*)oFm);
    char* pCur;
    for (i = 1; i < dwSubCount; i++)
    {
        pCur = pStart + auOffset[i];
        while (*pCur != '\n') pCur--;
        pCur++;
        auOffset[i] = (size_t)pCur - (size_t)pStart;
    }

    for (i = 0; i < dwSubCount; i++)
    {
        auSubSize[i] = auOffset[i + 1] - auOffset[i];
    }

    S_THREAD_PARAM* astTp = new S_THREAD_PARAM[dwThreadCount];
    pthread_t* auTid = new pthread_t[dwThreadCount];

    uint32_t dwWriteCount = dwSubCount / dwThreadCount;
    uint32_t dwStartI = 0;
    for (i = 0; i < dwThreadCount; i++)
    {
        astTp[i].pOut = pOut;
        astTp[i].pFileName = szFileName;
        astTp[i].pStart = pStart;
        astTp[i].pOffset = auOffset;
        astTp[i].pSubSize = auSubSize;
        astTp[i].dwStartI = dwStartI;
        dwStartI += dwWriteCount;
        if (dwStartI > dwSubCount)
        {
            dwStartI = dwSubCount;
        }
        astTp[i].dwEndI = dwStartI;
        if (pthread_create(&auTid[i], NULL, (void*(*)(void*))WriteThread, &astTp[i]) < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            return -1;
        }
    }
    for (i = 0; i < dwThreadCount; i++)
    {
        if (pthread_join(auTid[i], NULL) < 0)
        {
            PWriteString("= Failed | %s\n", strerror(errno));
            return -1;
        }
    }


    oFm.Close();
    delete[] auOffset;
    delete[] auSubSize;
    delete[] astTp;
    delete[] auTid;


    //PWriteString("= OK\n");
    return 0;
}

DEFCMDPROC(CmdAscFileSplit)
{
    strcat(const_cast<char*>(pParser->GetStringAddr()), " 1");
    return CmdAscFileSplitEx(pParser, pIn, pOut, sCmdName);
}


BEGIN_REGCMDS
REGCMD("ascfilesplit",
       CCommandInfo("split the large line text file.\n"
                    "usage: ascfilesplit FILE COUNT\n"),
       CmdAscFileSplit);
REGCMD("ascfilesplitx",
       CCommandInfo("split the large line text file with multi-thread.\n"
                    "usage: ascfilesplit FILE COUNT THREAD\n"),
       CmdAscFileSplitEx);
END_REGCMDS
