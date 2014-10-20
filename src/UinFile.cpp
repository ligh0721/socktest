#include "stdinc.h"
#include "UinFile.h"



// CUinFile

CUinFile::CUinFile()
: m_pFile(NULL)
, m_dwUin(0)
{
    
}

CUinFile::~CUinFile()
{
    Close();
}

bool CUinFile::Open(uint32_t dwUin, string sParentDir)
{
    Close();
    if (dwUin > MAX_UIN)
    {
        return false;
    }
    char szDir[16];
    char szFile[16];
    uint32_t dwA, dwB, dwC;
    dwA = dwUin / 1000000;
    dwB = (dwUin / 1000) % 1000;
    dwC = dwUin % 1000;
    sprintf(szDir, "%d/%d", dwA, dwB);
    sprintf(szFile, "%s/%d.uin", szDir, dwC);
    if ('/' != sParentDir[sParentDir.length() - 1])
    {
        sParentDir += "/";
    }
    system((string("mkdir -p ") + sParentDir + szDir).c_str());
    string sFullName = sParentDir + szFile;
    m_pFile = fopen64(sFullName.c_str(), "ab+");
    if (NULL == m_pFile)
    {
        perror(strerror(errno));
        return false;
    }
    return true;
}

void CUinFile::Close()
{
    if (NULL == m_pFile)
    {
        return;
    }
    fclose(m_pFile);
    m_pFile = NULL;
}

CUinFile::operator  FILE*() const
{
    return m_pFile;
}

uint32_t CUinFile::GetUin() const
{
    return m_dwUin;
}

// CBadgeFile

CBadgeFile::CBadgeFile()
{
    
}

CBadgeFile::~CBadgeFile()
{
    
}

bool CBadgeFile::AddScore(uint32_t dwUin)
{
    uint32_t dwCount;
    uint32_t dwExistUin;
    fseeko64(m_pFile, 0, SEEK_END);
    dwCount = ftell(m_pFile) / sizeof(uint32_t);
    if (dwCount >= MAX_SCORE)
    {
        return false;
    }
    fseeko64(m_pFile, 0, SEEK_SET);
    
    while (!feof(m_pFile))
    {
        fread(&dwExistUin, sizeof(dwExistUin), 1, m_pFile);
        if (dwUin == dwExistUin)
        {
            return false;
        }
    }
    fseeko64(m_pFile, 0, SEEK_END);
    fwrite(&dwUin, sizeof(uint32_t), 1, m_pFile);
    return true;
}

