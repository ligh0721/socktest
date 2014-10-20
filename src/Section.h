/* 
 * File:   Packer.h
 * Author: thunderliu
 *
 * Created on 2011年5月27日, 下午8:30
 */

#ifndef SECTION_H
#define	SECTION_H
using namespace std;

extern int g_iArgc;
extern const char** g_ppArgv;


uint64_t Switch(uint64_t a);
inline uint64_t htonll(uint64_t a) { return Switch(a); }
inline uint64_t ntohll(uint64_t a) { return Switch(a); }

class CIoInterface;

class CSectionBase;

typedef struct
{
    string sRawInput;
    CSectionBase* pSec;
    size_t uPos;
} SYMBOLVALUE;
typedef map<string, SYMBOLVALUE> SYMBOLVALUETABLE;
typedef map<string, CSectionBase*> SYMBOLTABLE;





class CSectionBase
{
protected:
    CSectionBase(string sName, size_t uSize);
    virtual ~CSectionBase();

public:
    static CSectionBase* CreateSectionBase(SYMBOLTABLE& roSym, string sName);
    static CSectionBase* FindSymbol(SYMBOLTABLE& roSym, string sName);
    static bool StrToValue(string sRawInput, void* pValBuf, size_t uSize);
    virtual size_t Serialize(void* pBuf, size_t uPos, FILE* pIn, FILE* pOut, string sPrefixName, string sCorrectName, SYMBOLTABLE& roSym, SYMBOLVALUETABLE& roGlbSymVal, bool bWrite, CIoInterface* pIo);
    bool SetValue(void* pBuf, size_t uPos, string sRawInput, SYMBOLVALUETABLE& roGlbSymVal);
    bool GetValue(void* pBuf, size_t uPos, char* pRawInput);

public:
    string m_sName;
    size_t m_uSize;

};

class CSectionStruct : public CSectionBase
{
public:
    typedef vector<CSectionBase*> SECSTINFO;
    typedef vector<string> ACTIONINFO;

    SECSTINFO m_oStInfo;
    SYMBOLTABLE m_oSym;
    ACTIONINFO m_oActInfo;

protected:
    typedef bool (CSectionStruct::*ACTION)(string sLine, void* pBuf, SYMBOLVALUETABLE& roGlbSymVal, FILE* pIn, FILE* pOut, CIoInterface* pIo);
    typedef map<string, ACTION> ACTIONTABLE;
    ACTIONTABLE m_oActMap;

protected:
    CSectionStruct(string sName);
    virtual ~CSectionStruct();

public:
    bool AddMember(SYMBOLTABLE& roSym, string sName);
    static CSectionStruct* CreateSectionStruct(SYMBOLTABLE& roSym, string sName);
    static CSectionStruct* CreateSectionStructFromLine(SYMBOLTABLE& roSym, string sLine);
    virtual size_t Serialize(void* pBuf, size_t uPos, FILE* pIn, FILE* pOut, string sPrefixName, string sCorrectName, SYMBOLTABLE& roSym, SYMBOLVALUETABLE& roGlbSymVal, bool bWrite, CIoInterface* pIo);
    virtual size_t Serialize(void* pBuf, FILE* pIn, FILE* pOut, SYMBOLTABLE& roSym, SYMBOLVALUETABLE& roGlbSymVal, bool bWrite, CIoInterface* pIo);

    void AddAction(string sLine);
    bool DoAction(void* pBuf, SYMBOLVALUETABLE& roGlbSymVal, FILE* pIn, FILE* pOut, CIoInterface* pIo);
    bool Patch(string sLine, void* pBuf, SYMBOLVALUETABLE& roGlbSymVal, FILE* pIn, FILE* pOut, CIoInterface* pIo);
};

class CSectionArray : public CSectionBase
{
public:

    class CSecArrInfo
    {
    public:

        CSecArrInfo(CSectionBase* pSec, string sCount) : m_pSec(pSec), m_sCount(sCount)
        {
        }

        CSecArrInfo(const CSecArrInfo& roArrInfo) : m_pSec(roArrInfo.m_pSec), m_sCount(roArrInfo.m_sCount)
        {
        }
        CSectionBase* m_pSec;
        string m_sCount;
    };
    CSecArrInfo m_oArrInfo;

protected:
    CSectionArray(string sName, const CSecArrInfo& roArrInfo);
    virtual ~CSectionArray();
public:

    static CSectionArray* CreateSectionArray(SYMBOLTABLE& roSym, string sName, SYMBOLTABLE& roGlbSym);
    virtual size_t Serialize(void* pBuf, size_t uPos, FILE* pIn, FILE* pOut, string sPrefixName, string sCorrectName, SYMBOLTABLE& roSym, SYMBOLVALUETABLE& roGlbSymVal, bool bWrite, CIoInterface* pIo);
};

class CFileMemMap
{
public:
    CFileMemMap();
    virtual ~CFileMemMap();
    bool Create(string sName, bool bReadOnly);
    bool Close();
    inline operator void*() const { return m_pAddr; }
    inline size_t GetSize() const { return m_uLen; }
    
protected:
    void* m_pAddr;
    size_t m_uLen;
    int m_iFd;
};

class CMemory
{
public:
    CMemory();
    virtual ~CMemory();
    bool Attach(void* pAddr, size_t uSize);
    int Read(void* pBuff, size_t uSize);
    void* Read(size_t uSize);
    int Write(const void* pBuff, size_t uSize);
    int Rewind();
    int Seek(long lOffset, int iBase);
    int Compare(void* pAddr) const;
    operator void*() const;
    size_t GetSize() const;
    size_t GetPos() const;
    void* GetAddr(size_t uOffset = 0) const;
    size_t FindFirst(void* pValue, size_t uSize, size_t uStart, size_t uEnd);
    size_t FindNext(void* pVaule, size_t uSize);
    
    void ShowHex() const;
    
    
protected:
    void* m_pAddr;
    size_t m_uSize;
    size_t m_uCur;
    size_t m_uFindCur;
    size_t m_uFindEnd;
};

#endif	/* SECTION_H */

