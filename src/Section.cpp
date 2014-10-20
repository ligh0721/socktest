#include "socktest.h"
#include "Section.h"

int g_iArgc;
const char** g_ppArgv;


#define swap_ddword(x)  \
   ((((x) & 0xff00000000000000llu) >> 56) | \
    (((x) & 0x00ff000000000000llu) >> 40) | \
    (((x) & 0x0000ff0000000000llu) >> 24) | \
    (((x) & 0x0000ff0000000000llu) >> 24) | \
    (((x) & 0x000000ff00000000llu) >> 8) | \
    (((x) & 0x00000000ff000000llu) << 8) | \
    (((x) & 0x0000000000ff0000llu) << 24) | \
    (((x) & 0x000000000000ff00llu) << 40) | \
    (((x) & 0x00000000000000ffllu) << 56) )

uint64_t Switch(uint64_t a)
{   
#if 0
    uint64_t iRet;
    int i;
    const unsigned char *psrc = NULL;
    unsigned char *pdst = NULL;
#endif

#if BYTE_ORDER == BIG_ENDIAN
    return a;
#elif BYTE_ORDER == LITTLE_ENDIAN
    #if 0
    psrc = (const unsigned char *)&a;
    pdst = (unsigned char *)&iRet;
    for (i = 0; i < 8; ++i)
    {
        pdst[i] = psrc[7-i];
    }
    return iRet;
    #else
    return swap_ddword(a);
    #endif
#else
#error "What kind of system is this?"
#endif
}


// CSectionBase

CSectionBase::CSectionBase(string sName, size_t uSize)
: m_sName(sName)
, m_uSize(uSize)
{

}

CSectionBase::~CSectionBase()
{

}

CSectionBase* CSectionBase::CreateSectionBase(SYMBOLTABLE& roSym, string sName)
{
    const char* pName = sName.c_str();
    size_t uSize;
    if (*(uint8_t*)"c" == *(uint8_t*)pName)
    {
        uSize = sizeof (uint8_t);
    }
    else if (*(uint8_t*)"w" == *(uint8_t*)pName)
    {
        uSize = sizeof (uint16_t);
    }
    else if (*(uint16_t*)"dw" == *(uint16_t*)pName)
    {
        uSize = sizeof (uint32_t);
    }
    else if (*(uint32_t*)"ddw" == ((*(uint32_t*)pName) & 0x00FFFFFF))
    {
        uSize = sizeof (uint64_t);
    }
    else
    {
        return NULL;
    }
    CSectionBase* pSec;
    pSec = CSectionBase::FindSymbol(roSym, sName);
    if (pSec)
    {
        // 符号重定义
        return pSec;
    }
    pSec = new CSectionBase(sName, uSize);
    if (!pSec)
    {
        LOG_POS("new mem err, %s", strerror(errno));
        return NULL;
    }
    roSym.insert(pair<string, CSectionBase*>(pSec->m_sName, pSec));
    return pSec;
}

CSectionBase* CSectionBase::FindSymbol(SYMBOLTABLE& roSym, string sName)
{
    SYMBOLTABLE::iterator it = roSym.find(sName);
    if (it == roSym.end())
    {
        return NULL;
    }
    return it->second;
}

bool CSectionBase::StrToValue(string sRawInput, void* pValBuf, size_t uSize)
{
    uint64_t uVal = 0;
    const char* pStr;
    if (!sRawInput.compare("dwTIME"))
    {
        uVal = time(NULL);
        memmove(pValBuf, &uVal, uSize);
        return true;
    }
    pStr = strchr(sRawInput.c_str(), 'x');
    if (pStr)
    {
        // hex
        if (!((pStr[1] <= '9' && pStr[1] >= '0') || (pStr[1] <= 'f' && pStr[1] >= 'a') || (pStr[1] <= 'F' && pStr[1] >= 'A')))
        {
            // error
            //LOG_POS("str(%s)", sRawInput.c_str());
            return false;
        }
#ifdef __x86_64__
        sscanf(pStr, "x%lx", &uVal);
#else
        sscanf(pStr, "x%llx", &uVal);
#endif
        memmove(pValBuf, &uVal, uSize);
        return true;
    }
    else if (sRawInput.length() > 1 && sRawInput[0] == '0')
    {
        // hex
        if (sRawInput[1] > '7' || sRawInput[1] < '0')
        {
            // error
            //LOG_POS("str(%s)", sRawInput.c_str());
            return false;
        }
#ifdef __x86_64__
        sscanf(sRawInput.c_str(), "%lo", &uVal);
#else
        sscanf(sRawInput.c_str(), "%llo", &uVal);
#endif
        memmove(pValBuf, &uVal, uSize);
        return true;
    }
    else if ((sRawInput[0] <= '9' && sRawInput[0] >= '0'))
    {
        uVal = atoll(sRawInput.c_str());
        memmove(pValBuf, &uVal, uSize);
        return true;
    }
    else if (sRawInput[0] == '%')
    {
        return StrToValue(g_ppArgv[atoi((sRawInput.c_str() + 1)) + 1], pValBuf, uSize);
    }
    else
    {
        // not surport
        //LOG_POS("str(%s) not suport", sRawInput.c_str());
        return false;
    }
    //LOG_POS("str(%s) not suport", sRawInput.c_str());
    return false;
}

size_t CSectionBase::Serialize(void* pBuf, size_t uPos, FILE* pIn, FILE* pOut, string sPrefixName, string sCorrectName, SYMBOLTABLE& roSym, SYMBOLVALUETABLE& roGlbSymVal, bool bWrite, CIoInterface* pIo)
{
    static int i = 0;
    static int n = 0;
    static char szRawInput[128];

    static char szRaw[128];
    const char* pStr;
    uint32_t dwLen = 0;
    char szBuf[128];
    string sFullName = sPrefixName + sCorrectName;
    
    if (bWrite)
    {

        fprintf(pOut, "%s:? ", sFullName.c_str());

        if (i != 0 && i < n)
        {
            // input loop
            if (szRaw[0] == '\"')
            {
                snprintf(szRawInput, sizeof(szRawInput), "%d", szRaw[i + 1]);
            }
            
            i++;
            fprintf(pOut, "\n");
        }
        else
        {
            if (i != 0 && i >= n)
            {
                i = 0;
                n = 0;
            }
            // normal
            //fscanf(pIn, "%s", szRaw);
            // input for serializing
            if (pIo->ReadString(szRaw, sizeof(szRaw)) == 0)
            {
                fprintf(pOut, "\n");
            }
            
            if (szRaw[0] == '\"')
            {
                n = strlen(szRaw) - 2;
                snprintf(szRawInput, sizeof(szRawInput), "%d", szRaw[1]);
                i = 1;
            }
            else if (pStr = strchr(szRaw, '['))
            {
                // "x100[100]"
                pStr = CStringHelper::PickOutWord(szRaw, 0, 0, "", "[ ", NULL, &dwLen);
                if (!pStr)
                {
                    // error
                }
                memmove(szRawInput, pStr, dwLen);
                szRawInput[dwLen] = 0;

                pStr = CStringHelper::PickOutWord(pStr + dwLen, 0, 0, "[ ", "] ", NULL, &dwLen);
                if (!pStr)
                {
                    // error
                }
                memmove(szBuf, pStr, dwLen);
                szBuf[dwLen] = 0;
                // 其它进制支持在此处添加
                if (!CSectionBase::StrToValue(szBuf, &n, sizeof (n)))
                {
                    // error
                    LOG_POS("str(%s) err", szBuf);
                    return -1;
                }
                i = 1;
            }
            else
            {
                strcpy(szRawInput, szRaw);
            }

        }
        if (!SetValue(pBuf, uPos, szRawInput, roGlbSymVal))
        {
            LOG_POS("str(%s) err", szRawInput);
            return -1;
        }
    }
    else
    {
        if (!GetValue(pBuf, uPos, szRawInput))
        {
            
            LOG_POS("str(%s) err", szRawInput);
            return -1;
        }

        fprintf(pOut, "0x%08x ", (uint32_t)uPos);  //, sFullName.c_str(), szRawInput);
        //fprintf(pOut, " %02X", ((uint8_t*)pBuf)[uPos]);
        for (int j = 0; j < m_uSize; j++)
        {
            fprintf(pOut, "%02X ", ((uint8_t*)pBuf)[uPos + j]);
        }
        for (int j = m_uSize; j < 8; j++)
        {
            fprintf(pOut, "   ");
        }
        
        fprintf(pOut, "%s: %s\n", sFullName.c_str(), szRawInput);
        
    }


    SYMBOLVALUE stSerInfo; // = new SERIALIZEINFO;
    stSerInfo.sRawInput = string(szRawInput);
    stSerInfo.uPos = uPos;
    stSerInfo.pSec = this;
    roGlbSymVal.insert(pair<string, SYMBOLVALUE > (sFullName, stSerInfo));
    
    sprintf(szBuf, "%lu", m_uSize);
    stSerInfo.sRawInput = string(szBuf);
    stSerInfo.pSec = CSectionBase::CreateSectionBase(roSym, "dwLENGTH");
    stSerInfo.uPos = -1;
    roGlbSymVal.insert(pair<string, SYMBOLVALUE > (sFullName + string(".dwLENGTH"), stSerInfo));

    return m_uSize;
}

bool CSectionBase::SetValue(void* pBuf, size_t uPos, string sRawInput, SYMBOLVALUETABLE& roGlbSymVal)
{
    char szBuf[128];
    SYMBOLVALUETABLE::iterator it;
    it = roGlbSymVal.find(sRawInput);
    if (it != roGlbSymVal.end())
    {
        // is sym, and exist
        uint64_t uVal = 0;
        if (!CSectionBase::StrToValue(it->second.sRawInput, &uVal, it->second.pSec->m_uSize))
        {
            // error
            return false;
        }
#ifdef __x86_64__
        sprintf(szBuf, "%lu", uVal);
#else
        sprintf(szBuf, "%llu", uVal);
#endif
        sRawInput = string(szBuf);
    }
    if (!CSectionBase::StrToValue(sRawInput, szBuf, sizeof (uint64_t)))
    {
        // error
        return false;
    }
    if ('@' != m_sName[m_sName.length() - 1])
    {
        switch (m_uSize)
        {
        case sizeof (uint16_t):
            *(uint16_t*)((uint8_t*)pBuf + uPos) = htons(*(uint16_t*)szBuf);
            break;
        case sizeof (uint32_t):
            *(uint32_t*)((uint8_t*)pBuf + uPos) = htonl(*(uint32_t*)szBuf);
            break;
        case sizeof(uint64_t):
            *(uint64_t*)((uint8_t*)pBuf + uPos) = htonll(*(uint64_t*)szBuf);
            break;
        default:
            // uint8_t
            memmove((uint8_t*)pBuf + uPos, szBuf, m_uSize);
        }
    }
    else
    {
        memmove((uint8_t*)pBuf + uPos, szBuf, m_uSize);
    }
    return true;
}

bool CSectionBase::GetValue(void* pBuf, size_t uPos, char* pRawInput)
{
    uint64_t uVal = 0;
    memmove(&uVal, (uint8_t*)pBuf + uPos, m_uSize);
    if ('@' != m_sName.c_str()[m_sName.length() - 1])
    {
        switch (m_uSize)
        {
        case sizeof (uint16_t):
            uVal = htons((uint16_t)uVal);
            break;
        case sizeof (uint32_t):
            uVal = htonl((uint32_t)uVal);
            break;
        case sizeof(uint64_t):
            uVal = htonll((uint64_t)uVal);
            break;
        default:
            // uint8_t
            //
            ;
        }
    }
    else
    {

    }
#ifdef __x86_64__
    sprintf(pRawInput, "%lu", uVal);
#else
    sprintf(pRawInput, "%llu", uVal);
#endif
    return true;
}

//CSectionStruct

CSectionStruct::CSectionStruct(string sName)
: CSectionBase(sName, 0)
{
    m_oActMap.insert(pair<string, ACTION > ("patch", &CSectionStruct::Patch));
}

CSectionStruct::~CSectionStruct()
{
}

bool CSectionStruct::AddMember(SYMBOLTABLE& roSym, string sName)
{
    CSectionBase* pSec;
    // 搜寻全局符号表
    pSec = CSectionBase::FindSymbol(roSym, sName);
    if (pSec)
    {
    }
    else
    {
        if (pSec = CSectionBase::CreateSectionBase(m_oSym, sName))
        {
            // 普通Sec
        }
        else if (pSec = CSectionStruct::CreateSectionStruct(roSym, sName))
        {
            // st
        }
        else if (pSec = CSectionArray::CreateSectionArray(m_oSym, sName, roSym))
        {
            // 数组
        }
        else
        {
            // error
            LOG_POS("member(%s) not suport", sName.c_str());
            return false;
        }
        // 将符号添加至局部符号表
        //m_oSym.insert(pair<string, CSectionBase*>(pSec->m_sName, pSec));
    }
    
    m_oStInfo.push_back(pSec);
    //fprintf(pOut, "<%s:%d>\n", m_sName.c_str(), m_pStInfo->size());
    return true;
}

CSectionStruct* CSectionStruct::CreateSectionStruct(SYMBOLTABLE& roSym, string sName)
{
    const char* pName = sName.c_str();
    if (*(uint16_t*)"st" != *(uint16_t*)pName)
    {
        return NULL;
    }
    CSectionBase* pSec;
    // 将stOther(...)括号中的规范化

    string sCorrectName = sName;
//#if 0
    char szBuf[256];
    uint32_t dwLen = 0;
    char* pStr;
    if (pStr = strchr(const_cast<char*>(sName.c_str()), '('))
    {
        // "x100[100]"
        pStr = CStringHelper::PickOutWord(sName.c_str(), 0, 0, "", "( ", NULL, &dwLen);
        if (!pStr)
        {
            // error
        }
        memmove(szBuf, pStr, dwLen);
        szBuf[dwLen] = 0;
        sCorrectName = string(szBuf);

        pStr = CStringHelper::PickOutWord(pStr + dwLen, 0, 0, "( ", ") ", NULL, &dwLen);
        if (!pStr)
        {
            // error
        }
        memmove(szBuf, pStr, dwLen);
        szBuf[dwLen] = 0;
        // 其它进制支持在此处添加
        if (dwLen)
        {
            // ()里面不是空，需要规范化
            uint32_t n = 0;
            if (CSectionBase::StrToValue(szBuf, &n, sizeof (n)))
            {
                snprintf(szBuf, sizeof(szBuf), "%d", n);
            }
            else
            {
                // error
                //LOG_POS("str(%s) err", szBuf);
                //return NULL;
                
            }
            
        }
        
        sCorrectName += string("(");
        sCorrectName += string(szBuf);
        sCorrectName += string(")");
    }
    
//#endif
    pSec = CSectionBase::FindSymbol(roSym, sCorrectName);
    if (pSec)
    {
        // 符号重定义
        return dynamic_cast<CSectionStruct*>(pSec);
    }
    // 创建新Struct
    pSec = new CSectionStruct(sCorrectName);
    if (!pSec)
    {
        LOG_POS("new mem err, %s", strerror(errno));
        return NULL;
    }
    roSym.insert(pair<string, CSectionBase*>(pSec->m_sName, pSec));
    return dynamic_cast<CSectionStruct*>(pSec);
}

CSectionStruct* CSectionStruct::CreateSectionStructFromLine(SYMBOLTABLE& roSym, string sLine)
{
    char* pStr;
    uint32_t dwLen = 0;
    char szBuf[1024];
    pStr = CStringHelper::PickOutWord(sLine.c_str(), 0, 0, "", "= ", NULL, &dwLen);
    memmove(szBuf, pStr, dwLen);
    szBuf[dwLen] = 0;
    CSectionStruct* pSec = CSectionStruct::CreateSectionStruct(roSym, szBuf);
    if (!pSec)
    {
        LOG_POS("struct\'s name(%s) err", szBuf);
        return NULL;
    }
    while (pStr = CStringHelper::PickOutWord(pStr + dwLen, 0, 0, "=+ ", "+\n ", NULL, &dwLen))
    {
        memmove(szBuf, pStr, dwLen);
        szBuf[dwLen] = 0;
        string sName = string(szBuf);
        pSec->AddMember(roSym, sName);
    }
    roSym.insert(pair<string, CSectionBase*>(pSec->m_sName, pSec));
    
    return pSec;
}

size_t CSectionStruct::Serialize(void* pBuf, size_t uPos, FILE* pIn, FILE* pOut, string sPrefixName, string sCorrectName, SYMBOLTABLE& roSym, SYMBOLVALUETABLE& roGlbSymVal, bool bWrite, CIoInterface* pIo)
{
    m_uSize = 0;
    int i;
    int n;
    char szBuf[128];
    uint32_t dwLen = 0;
    const char* pStr = sCorrectName.c_str();
    
    string sCorrectName2 = sCorrectName;
    
    // 判断是否含有括号
    pStr = strchr(pStr, '(');
    if (pStr)
    {
        // 将包括 ( 在内的sCorrectName之前的内容，复制到szCorrectName中
        char szCorrectName[128];
        dwLen = pStr - sCorrectName.c_str() + 1;
        memmove(szCorrectName, sCorrectName.c_str(), dwLen);
        szCorrectName[dwLen] = 0;
        // 取出括号之间的内容
        pStr = CStringHelper::PickOutWord(pStr, 0, 0, "( ", ") ", NULL, &dwLen);
        if (pStr)
        {
            memmove(szBuf, pStr, dwLen);
            szBuf[dwLen] = 0;
            
            // 在symval中查找括号之间的内容的值
            SYMBOLVALUETABLE::iterator it;
            it = roGlbSymVal.find(sPrefixName + szBuf);
            if (it != roGlbSymVal.end())
            {
                sCorrectName2 = string(szCorrectName) + it->second.sRawInput + string(")");
                CSectionBase* pSec = CSectionBase::FindSymbol(roSym, sCorrectName2);
                if (!pSec)
                {
                    // 尝试默认结构stOther()
                    sCorrectName2 = string(szCorrectName) + string(")");
                    pSec = CSectionBase::FindSymbol(roSym, sCorrectName2);
                    if (!pSec)
                    {
                        LOG_POS("%s not exist", sCorrectName2.c_str());
                        return -1;
                    }
                    //sCorrectName = sTmp;
                }
                return pSec->Serialize(pBuf, uPos, pIn, pOut, sPrefixName, pSec->m_sName, roSym, roGlbSymVal, bWrite, pIo);
            }
        }
    }
    n = m_oStInfo.size();
    string sFullName = sPrefixName + sCorrectName2;
    string sNewPrefixName = sFullName;
    if (!sNewPrefixName.empty())
    {
        sNewPrefixName += ".";
    }
    for (i = 0; i < n; i++)
    {
        m_uSize += m_oStInfo[i]->Serialize(pBuf, uPos + m_uSize, pIn, pOut, sNewPrefixName, m_oStInfo[i]->m_sName, roSym, roGlbSymVal, bWrite, pIo);
    }
    SYMBOLVALUE stSerInfo;
    
    sprintf(szBuf, "%lu", m_uSize);
    stSerInfo.sRawInput = string(szBuf);
    stSerInfo.pSec = CSectionBase::CreateSectionBase(m_oSym, "dwLENGTH");
    stSerInfo.uPos = -1;
    roGlbSymVal.insert(pair<string, SYMBOLVALUE > (sNewPrefixName + stSerInfo.pSec->m_sName, stSerInfo));
    if (bWrite)
    {
        DoAction(pBuf, roGlbSymVal, pIn, pOut, pIo);
    }

    return m_uSize;
}

size_t CSectionStruct::Serialize(void* pBuf, FILE* pIn, FILE* pOut, SYMBOLTABLE& roSym, SYMBOLVALUETABLE& roGlbSymVal, bool bWrite, CIoInterface* pIo)
{
    return Serialize(pBuf, 0, pIn, pOut, "", ""/*m_sName*/, roSym, roGlbSymVal, bWrite, pIo);
}

void CSectionStruct::AddAction(string sLine)
{
    m_oActInfo.push_back(sLine);
}

bool CSectionStruct::DoAction(void* pBuf, SYMBOLVALUETABLE& roGlbSymVal, FILE* pIn, FILE* pOut, CIoInterface* pIo)
{
    const char* pStr;
    uint32_t dwLen = 0;
    char szBuf[128];
    ACTIONTABLE::iterator it;
    int i;
    int n = m_oActInfo.size();
    for (i = 0; i < n; i++)
    {
        pStr = m_oActInfo[i].c_str();
        pStr = CStringHelper::PickOutWord(pStr, 0, 0, "@", " ", NULL, &dwLen);
        if (!pStr)
        {
            // error
            LOG_POS("act(%s) err", m_oActInfo[i].c_str());
            return false;
        }
        else
        {
            memmove(szBuf, pStr, dwLen);
            szBuf[dwLen] = 0;
            it = m_oActMap.find(szBuf);
            if (it == m_oActMap.end())
            {
                // error
                return false;
            }
            while (' ' == pStr[dwLen]) dwLen++;
            if (!(this->*(it->second))(pStr + dwLen, pBuf, roGlbSymVal, pIn, pOut, pIo))
            {
                // error
                return false;
            }
        }
    }
}

bool CSectionStruct::Patch(string sLine, void* pBuf, SYMBOLVALUETABLE& roGlbSymVal, FILE* pIn, FILE* pOut, CIoInterface* pIo)
{
    const char* pStr = sLine.c_str();
    const char* pVal;
    //printf(sLine.c_str());
    uint32_t dwLen = 0;
    char szBuf[128];
    char szBufVal[128];

    pStr = CStringHelper::PickOutWord(pStr, 0, 0, "", " =", NULL, &dwLen);
    if (!pStr)
    {
        // error
        return false;
    }
    memmove(szBuf, pStr, dwLen);
    szBuf[dwLen] = 0;
    fprintf(pOut, "%s:? ", szBuf);
    if (pVal = strchr(pStr + dwLen, '='))
    {
        pStr = CStringHelper::PickOutWord(pStr + dwLen, 0, 0, "= ", " ", NULL, &dwLen);
        if (!pStr)
        {
            // error
            return false;
        }
        memmove(szBufVal, pStr, dwLen);
        szBufVal[dwLen] = 0;
        fprintf(pOut, "%s\n", szBufVal);
    }
    else
    {
        // 如果这一行没有等号，表示需要用户输入
        //readline()
        //fscanf(pIn, "%s", szBufVal);
        if (pIo->ReadString(szBufVal, sizeof(szBufVal)) == 0)
        {
            fprintf(pOut, "\n");
        }
    }

    SYMBOLVALUETABLE::iterator it;
    it = roGlbSymVal.find(szBuf);
    if (it == roGlbSymVal.end())
    {
        // error
        return false;
    }
    if (!it->second.pSec->SetValue(pBuf, it->second.uPos, szBufVal, roGlbSymVal))
    {
        // errror
        return false;
    }
    return true;
}


// CSectionArray

CSectionArray::CSectionArray(string sName, const CSecArrInfo& roArrInfo)
: CSectionBase(sName, 0)
, m_oArrInfo(roArrInfo)
{
}

CSectionArray::~CSectionArray()
{

}

CSectionArray* CSectionArray::CreateSectionArray(SYMBOLTABLE& roSym, string sName, SYMBOLTABLE& roGlbSym)
{
    const char* pName = sName.c_str();
    if ('a' != *(uint8_t*)pName)
    {
        return NULL;
    }
    char* pStr;
    uint32_t dwLen = 0;
    char szBuf[128];
    CSectionBase* pSubSec;
    CSectionBase* pSec;
    string sCorrectName;
    // a和[中间的部分
    pStr = CStringHelper::PickOutWord(pName, 0, 0, "a", "[ ", NULL, &dwLen);
    if (!pStr)
    {
        // error
        return NULL;
    }
    memmove(szBuf, pStr, dwLen);
    szBuf[dwLen] = 0;
    sCorrectName = string("a") + szBuf;

    if ('@' == sName[sName.length() - 1])
    {
        szBuf[dwLen] = '@';
        dwLen++;
        szBuf[dwLen] = 0;
    }

    pSubSec = CSectionBase::CreateSectionBase(roGlbSym, szBuf);
    if (!pSubSec)
    {
        pSubSec = CSectionStruct::CreateSectionStruct(roGlbSym, szBuf);
        if (!pSubSec)
        {
            // error
            LOG_POS("create array subsec create err");
            return NULL;
        }
    }
    pStr = CStringHelper::PickOutWord(pStr + dwLen, 0, 0, "[ ", "] ", NULL, &dwLen);
    if (!pStr)
    {
        // error
        LOG_POS("err between [ and ]");
        return NULL;
    }
    memmove(szBuf, pStr, dwLen);
    szBuf[dwLen] = 0;
    pSec = CSectionBase::FindSymbol(roSym, sCorrectName);
    if (pSec)
    {
        // 符号重定义
        return dynamic_cast<CSectionArray*>(pSec);
    }
    pSec = new CSectionArray(sCorrectName, CSecArrInfo(pSubSec, szBuf));
    roSym.insert(pair<string, CSectionBase*>(pSec->m_sName, pSec));
    return dynamic_cast<CSectionArray*>(pSec);
}

size_t CSectionArray::Serialize(void* pBuf, size_t uPos, FILE* pIn, FILE* pOut, string sPrefixName, string sCorrectName, SYMBOLTABLE& roSym, SYMBOLVALUETABLE& roGlbSymVal, bool bWrite, CIoInterface* pIo)
{
    m_uSize = 0;
    uint64_t i;
    uint64_t n;
    char szBuf[128];
    const char* pStr;
    const char* pSuffix = "";
    pStr = m_oArrInfo.m_sCount.c_str();
    if ((pStr[0] >= '0' && pStr[0] <= '9') || pStr[0] == 'x')
    {
        // 其它进制数支持在此处添加
        if (!CSectionBase::StrToValue(pStr, &n, sizeof (n)))
        {
            // error
            LOG_POS("arr count(%s) err", pStr);
            return -1;
        }
    }
    else
    {
        // 在Global的roGlbSymVal里确定n的值
        SYMBOLVALUETABLE::iterator it;
        it = roGlbSymVal.find(sPrefixName + m_oArrInfo.m_sCount);
        if (it == roGlbSymVal.end())
        {
            // error
            LOG_POS("ser arr count\'s val not found");
            return -1;
        }
        pStr = it->second.sRawInput.c_str();

        // sRawInput 其它进制数支持在此处添加
        if (!CSectionBase::StrToValue(pStr, &n, sizeof (n)))
        {
            // error
            LOG_POS("str(%s) err", pStr);
            return -1;
        }
    }
    //printf("%d:%c", m_oArrInfo.m_pSec->m_sName.at(m_oArrInfo.m_pSec->m_sName.length() - 1), m_oArrInfo.m_pSec->m_sName.at(m_oArrInfo.m_pSec->m_sName.length() - 1));
    if ('@' == m_oArrInfo.m_pSec->m_sName[m_oArrInfo.m_pSec->m_sName.length() - 1])
    {
        pSuffix = "@";
    }

    for (i = 0; i < n; i++)
    {
#ifdef __x86_64__
        sprintf(szBuf, "%lu", i);
#else
        sprintf(szBuf, "%llu", i);
#endif
        m_uSize += m_oArrInfo.m_pSec->Serialize(pBuf, uPos + m_uSize, pIn, pOut, sPrefixName, sCorrectName + "[" + szBuf + "]" + pSuffix, roSym, roGlbSymVal, bWrite, pIo);
    }
    
    SYMBOLVALUE stSerInfo;
    sprintf(szBuf, "%lu", m_uSize);
    stSerInfo.sRawInput = string(szBuf);
    stSerInfo.pSec = CSectionBase::CreateSectionBase(roSym, "dwLENGTH");
    stSerInfo.uPos = -1;
    roGlbSymVal.insert(pair<string, SYMBOLVALUE > (sPrefixName + sCorrectName + ".dwLENGTH", stSerInfo));
    
    return m_uSize;
}

// CFileMemMap

CFileMemMap::CFileMemMap()
: m_pAddr(MAP_FAILED)
, m_uLen(0)
, m_iFd(-1)
{
    
}

CFileMemMap::~CFileMemMap()
{
    Close();
}

bool CFileMemMap::Create(string sName, bool bReadOnly)
{
    Close();
    m_iFd = open(sName.c_str(), bReadOnly ? O_RDONLY : O_RDWR);
    if (m_iFd < 0)
    {
        LOG_POS("%s", strerror(errno));
        return false;
    }
    struct stat stStat;
    __fxstat(_STAT_VER, m_iFd, &stStat);
    m_uLen = stStat.st_size;
    m_pAddr = mmap(NULL, m_uLen, bReadOnly ? PROT_READ : PROT_READ | PROT_WRITE, bReadOnly ? MAP_PRIVATE | MAP_NORESERVE : MAP_SHARED, m_iFd, 0);
    close(m_iFd);
    if (m_pAddr == MAP_FAILED)
    {
        LOG_POS("%s", strerror(errno));
        return false;
    }
    return true;
}

bool CFileMemMap::Close()
{
    if (m_pAddr == MAP_FAILED)
    {
        return false;
    }
    munmap(m_pAddr, m_uLen);
    m_pAddr = MAP_FAILED;
    return true;
}


