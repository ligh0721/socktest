/* 
 * File:   CmdParser.h
 * Author: thunderliu
 *
 * Created on 2011年5月20日, 下午3:58
 */

#ifndef CMDPARSER_H
#define	CMDPARSER_H

using namespace std;

int fscanfln(FILE* pIn);


class CCmdParser;

#define DEFCMDPROC(name) int name(CCmdParser* pParser, FILE* pIn, FILE* pOut, string sCmdName)
typedef DEFCMDPROC((*CMDPROC));

typedef void (*REGCMDSPROC)(CCmdParser* pParser, FILE* pIn, FILE* pOut);

class CCommandInfo
{
public:
    CCommandInfo(const string& rDoc): sDoc(rDoc) {}
    //string sName;
    string sDoc;
};


class CIoInterface
{
public:
    virtual int SetOrGetIo(FILE* pIn, FILE* pOut, FILE** ppOrgIn = NULL, FILE** ppOrgOut = NULL) = 0;
    virtual int ReadString(char* pStr, uint32_t dwMaxSize) = 0;
    virtual int ReadDword(uint32_t& dwVal) = 0;
    virtual int ReadQword(uint64_t& ddwVal) = 0;
    virtual int ReadSize(size_t& uVal) = 0;
};

class CStringHelper : public CIoInterface
{
public:
    CStringHelper(const char* pLine = NULL): m_pLine(pLine), m_pPos(m_pLine), m_bIsEnd(!m_pLine) {}
    static char* PickOutWord(const char* pStr, char cLeftPair, char cRightPair, const char* pLeftSeps, const char* pRightSeps, char* pSep, uint32_t* pWordLen);
    
    inline void Attach(const char* pLine) { m_pLine = pLine; m_pPos = m_pLine; m_bIsEnd = !m_pLine; }
    virtual const char* GetNextStringPos(uint32_t& dwLen);
    
    virtual int SetOrGetIo(FILE* pIn, FILE* pOut, FILE** ppOrgIn = NULL, FILE** ppOrgOut = NULL);
    virtual int ReadString(char* pStr, uint32_t dwMaxSize);
    virtual int ReadDword(uint32_t& dwVal);
    virtual int ReadQword(uint64_t& ddwVal);
    virtual int ReadSize(size_t& uVal);
    
    const char* GetPos();
    inline const char* GetStringAddr() const { return m_pLine; }
    inline bool IsEnd() { return m_bIsEnd; }
    
protected:
    const char* m_pLine;
    const char* m_pPos;
    bool m_bIsEnd;

};


class CCmdStringHelper : public CStringHelper
{
public:
    CCmdStringHelper(int iArgc = 0, const char** ppArgv = NULL): m_iArgc(iArgc), m_ppArgv(ppArgv) {}
    
    virtual int ReadString(char* pStr, uint32_t dwMaxSize);
    inline void SetArg(int iArgc, const char** ppArgv) { m_iArgc = iArgc; m_ppArgv = ppArgv; }
    inline int GetArgc() const { return m_iArgc; }
    inline const char** GetArgv() const { return m_ppArgv; }    
    
    
protected:
    int m_iArgc;
    const char** m_ppArgv;
};


class CIoReadline : public CCmdStringHelper
{
public:
    CIoReadline();
    inline static CIoReadline* Instance() { return CIoReadline::m_pInstance; }
    inline static void Instance(CIoReadline* pInstance) { CIoReadline::m_pInstance = pInstance; }
    
    virtual int SetOrGetIo(FILE* pIn, FILE* pOut, FILE** ppOrgIn = NULL, FILE** ppOrgOut = NULL);
    
    virtual int ReadString(char* pStr, uint32_t dwMaxSize);
    inline int ReadStringEnd(char* pStr, uint32_t dwMaxSize) { return CCmdStringHelper::ReadString(pStr, dwMaxSize); }
    
    void AddToReadline(string sName, const CCommandInfo& rCmd);
    string GetCommandInfo(string sName);
    inline void SetPrompt(string sPrompt) { m_sPrompt = sPrompt; }
    int Readline();
    
    
protected:
    static char* CommandGenerator(const char* pText, int iState);
    static char** CommandCompletion(const char* pText, int iStart, int iEnd);
    static void InitializeReadline();
    
protected:
    static CIoReadline* m_pInstance;
    map<string, CCommandInfo> m_mapCmd;
    string m_sPrompt;
    
    
};

class CCmdParser : public CIoReadline
{
public:
    typedef enum
    {
        RM_INTERACTIVE,
        RM_BATCH
    } RUNMODE;
    
public:
    CCmdParser();
    void RegCmd(string sName, const CCommandInfo& rCmd, CMDPROC fnCmdProc);
    void ParseLoop(FILE* pIn, FILE* pOut, int iArgc, const char** ppArgv);
    void ParseLine(FILE* pIn, FILE* pOut);
    inline void Quit() { m_bContinue = false; }
    inline void Continue(bool bContinue = true) { m_bContinue = bContinue; }
    inline void AutoShell(bool bAutoShell = true) { m_bAutoShell = bAutoShell; }
    inline void RunMode(RUNMODE eRunMode) { m_eRunMode = eRunMode; }
    inline RUNMODE RunMode() const { return m_eRunMode; }

protected:
    map<string, CMDPROC> m_oCmds;
    bool m_bContinue;
    bool m_bAutoShell;
    RUNMODE m_eRunMode;
    
        
};


#endif	/* CMDPARSER_H */

