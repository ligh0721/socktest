#ifndef SOCKET_H
#define	SOCKET_H


struct etharphdr
{
    struct arphdr       ar_hdr;
    unsigned char		ar_sha[ETH_ALEN];	/* sender hardware address	*/
	unsigned char		ar_sip[4];		/* sender IP address		*/
	unsigned char		ar_tha[ETH_ALEN];	/* target hardware address	*/
	unsigned char		ar_tip[4];		/* target IP address		*/
};


class CSockAddr
{
public:
    CSockAddr(const struct sockaddr* pstSockAddr, socklen_t dwSockLen);
    CSockAddr(const CSockAddr& roSockAddr);
    ~CSockAddr();
    operator struct sockaddr*();
    operator const struct sockaddr*() const;
    void SetSockAddr(const struct sockaddr* pstSockAddr);
    CSockAddr& operator =(const CSockAddr& roSockAddr);
    socklen_t GetSockLen() const;
    
protected:
    struct sockaddr* m_pstSockAddr;
    socklen_t m_dwSockLen;
};

class CSockAddrIn : public CSockAddr
{
public:
    CSockAddrIn(const struct sockaddr* pstSockAddr = NULL);
    CSockAddrIn(const char* pAddr, int iPort);
    
    void SetSockAddrIn(const char* pAddr, int iPort);
    
    static void MakeSockAddrInStruct(struct sockaddr* pstSockAddr, const char* pAddr, int iPort);
    
    operator struct sockaddr_in*();
    operator const struct sockaddr_in*() const;
    
    char* GetAddr();
    int GetPort();
    static CSockAddrIn* ConvertToSockAddrIn(const struct sockaddr* pstSockAddr);
    static CSockAddrIn* ConvertToSockAddrIn(const char* pAddr, int iPort);
    

protected:
    static CSockAddrIn m_oConvert;
    
};

class CSockAddrLl : public CSockAddr
{
public:
    CSockAddrLl(const struct sockaddr* pstSockAddr = NULL);
    CSockAddrLl(const char* pInterface, const char* pHardAddr);
    
    void SetSockAddrLl(const char* pInterface, const char* pHardAddr);
    
    static void MakeSockAddrLlStruct(struct sockaddr* pstSockAddr, const char* pInterface, const char* pHardAddr);
    
    operator struct sockaddr_ll*();
    operator const struct sockaddr_ll*() const;
    
    char* GetAddr();
    //int GetPort();
    
    static CSockAddrLl* ConvertToSockAddrLl(const struct sockaddr* pstSockAddr);
    static CSockAddrLl* ConvertToSockAddrLl(const char* pInterface, const char* pHardAddr);
    
protected:
    static CSockAddrLl m_oConvert;
};


class CSocket
{
public:
    CSocket(int iSock = -1);
    virtual ~CSocket();
    bool Bind(const CSockAddr* pSockAddr);
    CSockAddr* GetSockAddr();
    bool SetSockOpt(int iLevel, int iOptName, const void* pOptVal, socklen_t dwOptlen);
    bool SetRecvTimeout(int iTimeout);
    
    virtual bool Close();
    int Send(const void* pBuf, int iSize);
    int Recv(void* pBuf, int iSize);
    int SendTo(const void* pBuf, int iSize, const CSockAddr* pSockAddr);
    int RecvFrom(void* pBuf, int iSize, CSockAddr* pSockAddr);
    
    operator int() const;
    
protected:
    virtual bool CreateSocket() = 0;
    
    
protected:
    CSockAddr* m_pSockAddr;
    int m_iFd;
};

class CIpSocket : public CSocket
{
public:
    CIpSocket(int iSock = -1);
    virtual ~CIpSocket();
    
    bool Bind(const char* pAddr, int iPort, bool bReuseAddr = false);

};

class CUdpSocket : public CIpSocket
{
public:
    CUdpSocket(int iSock = -1);
    
protected:
    virtual bool CreateSocket();

};

class CTcpSocket : public CIpSocket
{
public:
    CTcpSocket(int iSock = -1);
    
    bool Connect(const CSockAddr* pSockAddr);
    bool Connect(const char* pAddr, int iPort);
    bool Listen(int iBacklog = 5);
    bool Accept(CTcpSocket* pStreamSock);
    bool SendEx(const void* pBuf, int iSize);
    bool RecvEx(void* pBuf, int iSize);
    
protected:
    virtual bool CreateSocket();
};

class CEthSocket : public CSocket
{
public:
    CEthSocket(int iSock = -1);
    bool Bind(const char* pInterface);
    bool SetPromisc(const char* pInterface, bool bOn);
    
protected:
    virtual bool CreateSocket();
};



#endif	/* SOCKET_H */

