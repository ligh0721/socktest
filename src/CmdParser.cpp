/* 
 * File:   CmdParser.cpp
 * Author: thunderliu
 * 
 * Created on 2011年5月20日, 下午3:58
 */


#include "socktest.h"
#include "CmdParser.h"


int fscanfln(FILE* pIn)
{
    static char _szTmp[1024];
    return fscanf(pIn, "%[\r\n]", _szTmp);
}






// CStringHelper
char* CStringHelper::PickOutWord(const char* pStr, char cLeftPair, char cRightPair, const char* pLeftSeps, const char* pRightSeps, char* pSep, uint32_t* pWordLen)
{
    if (!pStr)
    {
        return NULL;
    }
    uint32_t i, j, k;
    bool bSepFlag = false;
    int iPairPos = -1;
    for (i = 0; pStr[i]; i++)
    {
        if (iPairPos != -1)
        {
            if (pStr[i] == cRightPair)
            {
                if (pSep) *pSep = cRightPair;
                if (pWordLen) *pWordLen = i + 1 - iPairPos;
                if (i + 1 - iPairPos == 0)
                {
                    return NULL;
                }
                return (char*)&pStr[iPairPos];
            }
            else
            {
                continue;
            }
        }
        else if (pStr[i] == cLeftPair)
        {
            iPairPos = i;
            continue;
        }
        // ignore left seps
        bSepFlag = false;
        for (j = 0; pLeftSeps[j]; j++)
        {
            if (pStr[i] == pLeftSeps[j])
            {
                bSepFlag = true;
                break;
            }
        }
        if (bSepFlag)
        {
            continue;
        }
        
        // not a leftsep
        for (k = i; pStr[k]; k++)
        {
            for (j = 0; pRightSeps[j]; j++)
            {
                if (pStr[k] == pRightSeps[j])
                {
                    if (pSep) *pSep = pRightSeps[j];
                    if (pWordLen) *pWordLen = k - i;
                    if (k - i == 0)
                    {
                        return NULL;
                    }
                    return (char*)&pStr[i];
                }
            }
        }
        if (pSep) *pSep = 0;
        if (pWordLen) *pWordLen = k - i;
        return (char*)&pStr[i];
    }
    return NULL;
}

const char* CStringHelper::GetNextStringPos(uint32_t& dwLen)
{
    m_pPos = CStringHelper::PickOutWord(m_pPos, '\"', '\"', " \t\r\n", " \t\r\n", NULL, &dwLen);
    if (!m_pPos)
    {
        m_bIsEnd = true;
        return NULL;
    }
    const char* pRet = m_pPos;
    m_pPos += dwLen;
    return pRet;
}

int CStringHelper::SetOrGetIo(FILE* pIn, FILE* pOut, FILE** ppOrgIn, FILE** ppOrgOut)
{
    return 0;
}

int CStringHelper::ReadString(char* pStr, uint32_t dwMaxSize)
{
    uint32_t dwLen;
    const char* pRet = GetNextStringPos(dwLen);
    if (!pRet)
    {
        return -1;
    }
    uint32_t dwStrLen = dwLen >= dwMaxSize ? dwMaxSize - 1 : dwLen;
    memmove(pStr, pRet, dwStrLen);
    pStr[dwStrLen] = 0;
    return 0;
}


int CStringHelper::ReadDword(uint32_t& dwVal)
{
    char szTmp[32];
    if (ReadString(szTmp, sizeof(szTmp)) < 0)
    {
        return -1;
    }
    int iRet;
    if (szTmp[0] == '0')
    {
        if (szTmp[1] == 'x')
        {
            iRet = sscanf(szTmp, "%x", &dwVal);
        }
        else
        {
            iRet = sscanf(szTmp, "%o", &dwVal);
        }

    }
    else if (szTmp[0] == 'x')
    {
        iRet = sscanf(szTmp, "x%x", &dwVal);
    }
    else
    {
        iRet = sscanf(szTmp, "%u", &dwVal);
    }
    return iRet ? 0 : -1;
}

int CStringHelper::ReadQword(uint64_t& ddwVal)
{
    char szTmp[32];
    if (ReadString(szTmp, sizeof(szTmp)) < 0)
    {
        return -1;
    }
    int iRet;
    if (szTmp[0] == '0')
    {
        if (szTmp[1] == 'x')
        {
#ifdef __x86_64__
            iRet = sscanf(szTmp, "%lx", &ddwVal);
#else
            iRet = sscanf(szTmp, "%llx", &ddwVal);
#endif
        }
        else
        {
#ifdef __x86_64__
            iRet = sscanf(szTmp, "%lo", &ddwVal);
#else
            iRet = sscanf(szTmp, "%llo", &ddwVal);
#endif
        }

    }
    else if (szTmp[0] == 'x')
    {
#ifdef __x86_64__
        iRet = sscanf(szTmp, "x%lx", &ddwVal);
#else
        iRet = sscanf(szTmp, "x%llx", &ddwVal);
#endif
    }
    else
    {
#ifdef __x86_64__
        iRet = sscanf(szTmp, "%lu", &ddwVal);
#else
        iRet = sscanf(szTmp, "%llu", &ddwVal);
#endif
    }
    return iRet ? 0 : -1;
}

int CStringHelper::ReadSize(size_t& uVal)
{
    char szTmp[32];
    if (ReadString(szTmp, sizeof(szTmp)) < 0)
    {
        return -1;
    }
    int iRet;
    if (szTmp[0] == '0')
    {
        if (szTmp[1] == 'x')
        {
            iRet = sscanf(szTmp, "%zx", &uVal);
        }
        else
        {
            iRet = sscanf(szTmp, "%zo", &uVal);
        }

    }
    else if (szTmp[0] == 'x')
    {
        iRet = sscanf(szTmp, "x%zx", &uVal);
    }
    else
    {
        iRet = sscanf(szTmp, "%zu", &uVal);
    }
    return iRet ? 0 : -1;
}

const char* CStringHelper::GetPos()
{
    while (*m_pPos == ' ' || *m_pPos == '\t' || *m_pPos == '\r' || *m_pPos == '\n')
    {
        m_pPos++;
    }
    return m_pPos;
}

// CCmdStringHelper
int CCmdStringHelper::ReadString(char* pStr, uint32_t dwMaxSize)
{
    char szBuff[1024];
    if (CStringHelper::ReadString(szBuff, sizeof(szBuff)) < 0)
    {
        return -1;
    }
    const char* pRet;
    uint32_t dwLen;
    if (szBuff[0] == '%')
    {
        pRet = m_ppArgv[atoi(szBuff + 1) + 1];
    }
    else
    {
        pRet = szBuff;
    }
    dwLen = strlen(pRet);
    uint32_t dwStrLen = dwLen >= dwMaxSize ? dwMaxSize - 1 : dwLen;
    memmove(pStr, pRet, dwStrLen);
    pStr[dwStrLen] = 0;
    return 0;
}

// CIoReadline
CIoReadline* CIoReadline::m_pInstance = NULL;

CIoReadline::CIoReadline()
{
    m_pInstance = this;
    InitializeReadline();
}

int CIoReadline::Readline()
{
    char* pCmdLn;
    static char szCmdLn[10240] = {0};

    pCmdLn = readline(m_sPrompt.c_str());
    if (!pCmdLn)
    {
        
        return -1;
    }
//    if (!(*pCmdLn))
//    {
//        return;
//    }
    if (strncmp(pCmdLn, szCmdLn, sizeof(szCmdLn)))
    {
        strncpy(szCmdLn, pCmdLn, sizeof(szCmdLn));
        add_history(pCmdLn);
    }

    Attach(pCmdLn);
    return 0;
}

int CIoReadline::ReadString(char* pStr, uint32_t dwMaxSize)
{
    int iRet = 0;
    while (CCmdStringHelper::ReadString(pStr, dwMaxSize) < 0)
    {
        iRet = 1;
        if (Readline() < 0)
        {
            iRet = -1;
            break;
        }
    }
    
    return iRet;
}

int CIoReadline::SetOrGetIo(FILE* pIn, FILE* pOut, FILE** ppOrgIn, FILE** ppOrgOut)
{
    if (ppOrgIn)
    {
        *ppOrgIn = rl_instream;
    }

    if (ppOrgOut)
    {
        *ppOrgOut = rl_outstream;
    }

    if (pIn)
    {
        rl_instream = pIn;
    }
    
    if (pOut)
    {
        rl_outstream = pOut;
    }

    return 0;
}

char* CIoReadline::CommandGenerator(const char* pText, int iState)
{
    const char* pName;
    static int iLen, iCount;
    static map<string, CCommandInfo>::iterator it;
    if (!iState)
    {
        it = m_pInstance->m_mapCmd.begin();
        iLen = strlen(pText);
        iCount = m_pInstance->m_mapCmd.size();
    }

    while (it != m_pInstance->m_mapCmd.end())
    {
        pName = it->first.c_str();
        it.operator ++();

        if (!strncmp(pName, pText, iLen))
        {
            return strdup(pName);
        }
    }

    return NULL;

}

char** CIoReadline::CommandCompletion(const char* pTest, int iStart, int iEnd)
{
    char** pRet = NULL;
    if (!iStart)
    {
        pRet = rl_completion_matches(pTest, CIoReadline::CommandGenerator);
    }
    return pRet;
}

void CIoReadline::InitializeReadline()
{
    rl_readline_name = "CIoReadline";
    rl_attempted_completion_function = CIoReadline::CommandCompletion;
}

void CIoReadline::AddToReadline(string sName, const CCommandInfo& rCmd)
{
    if (!sName.empty())
    {
        m_mapCmd.insert(pair<string, CCommandInfo>(sName, rCmd));
    }
}

string CIoReadline::GetCommandInfo(string sName)
{
    map<string, CCommandInfo>::iterator it = m_mapCmd.find(sName);
    if (it != m_mapCmd.end())
    {
        return it->second.sDoc;
    }
    return string();
}


// CCmdParser
CCmdParser::CCmdParser()
: m_bContinue(false)
, m_bAutoShell(true)
{
    SetPrompt("> ");
}

void CCmdParser::ParseLoop(FILE* pIn, FILE* pOut, int iArgc, const char** ppArgv)
{
    FILE* pOrgIn;
    FILE* pOrgOut;
    
    SetOrGetIo(pIn, pOut, &pOrgIn, &pOrgOut);
    SetArg(iArgc, ppArgv);
    m_bContinue = true;
    while (!feof(pIn) && m_bContinue)
    {
        ParseLine(pIn, pOut);
    }
    SetOrGetIo(pOrgIn, pOrgOut);
}

void CCmdParser::RegCmd(string sName, const CCommandInfo& rCmd, CMDPROC fnCmdProc)
{
    m_oCmds.insert(pair<string, CMDPROC>(sName, fnCmdProc));
    AddToReadline(sName, rCmd);
}

void CCmdParser::ParseLine(FILE* pIn, FILE* pOut)
{
    char* pCmdLn;
    static char szCmdLn[10240] = {0};
    char szCmd[128];
    
    //pCmdLn = readline(m_sPrompt.c_str());
    if (Readline() < 0)
    {
        m_bContinue = false;
        fprintf(pOut, "\n");
        return;
    }
    
//    if (!pCmdLn)
//    {
//        m_bContinue = false;
//        fprintf(pOut, "\n");
//        return;
//    }
//    if (!(*pCmdLn))
//    {
//        return;
//    }
//    if (strcmp(pCmdLn, szCmdLn))
//    {
//        strcpy(szCmdLn, pCmdLn);
//        add_history(pCmdLn);
//    }
//    
//    Attach(pCmdLn);
    if (ReadString(szCmd, sizeof(szCmd)) < 0)
    {
        return;
    }
    
    map<string, CMDPROC>::iterator it = m_oCmds.find(szCmd);
    if (it != m_oCmds.end())
    {
        // unknown command
		if (it->second(this, pIn, pOut, szCmd) < 0)
        {
            // 执行失败
            if (m_eRunMode == RM_BATCH)
            {
                // 是批处理模式，quit loop
                m_bContinue = false;
            }
        }
    }
    else
    {
        // unknown
        if (m_bAutoShell)
        {
            // run as shell
            it = m_oCmds.find("");
            if (it != m_oCmds.end())
            {
                // 找到默认处理命令
                if (it->second(this, pIn, pOut, szCmd) < 0)
                {
                    // 默认处理执行失败
                    if (m_eRunMode == RM_BATCH)
                    {
                        // 是批处理模式，quit loop
                        m_bContinue = false;
                    }
                }
            }
            else
            {
                // 没有找到默认处理命令
                if (m_eRunMode == RM_BATCH)
                {
                    // 是批处理模式，quit loop
                    m_bContinue = false;
                }
                fprintf(pOut, "= Failed | default command was not specified.\n");
            }
        }
        else
        {
            // autoshell == 0
            if (m_eRunMode == RM_BATCH)
            {
                // 是批处理模式，quit loop
                m_bContinue = false;
            }
            fprintf(pOut, "= Failed | unknown command.\n");
        }
    }
    
}

