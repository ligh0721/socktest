/* 
 * File:   UinFile.h
 * Author: thunderliu
 *
 * Created on 2011年6月3日, 上午10:29
 */

#ifndef UINFILE_H
#define	UINFILE_H

using namespace std;

class CUinFile
{
public:
    static const uint32_t MAX_UIN = 999999999;
    
public:
    CUinFile();
    virtual ~CUinFile();
    bool Open(uint32_t dwUin, string sParentDir);
    void Close();
    operator FILE*() const;
    uint32_t GetUin() const;
    
protected:
    FILE* m_pFile;
    uint32_t m_dwUin;
};

class CBadgeFile : public CUinFile
{
public:
    static const uint32_t MAX_SCORE = 5;
public: 
    CBadgeFile();
    virtual ~CBadgeFile();
    bool AddScore(uint32_t dwUin);
    
};



#endif	/* UINFILE_H */

