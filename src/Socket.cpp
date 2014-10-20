#include "socktest.h"
#include "Socket.h"

// CSockAddr

CSockAddr::CSockAddr(const struct sockaddr* pstSockAddr, socklen_t dwSockLen)
{
    m_dwSockLen = dwSockLen;
    if (!pstSockAddr)
    {
        m_pstSockAddr = NULL;
        return;
    }
    m_pstSockAddr = (struct sockaddr*)malloc(m_dwSockLen);
    memmove(m_pstSockAddr, pstSockAddr, m_dwSockLen);
}

CSockAddr::CSockAddr(const CSockAddr& roSockAddr)
{
    m_dwSockLen = roSockAddr.m_dwSockLen;
    if (!roSockAddr.m_pstSockAddr)
    {
        m_pstSockAddr = NULL;
        return;
    }
    m_pstSockAddr = (struct sockaddr*)malloc(m_dwSockLen);
    memmove(m_pstSockAddr, roSockAddr.m_pstSockAddr, m_dwSockLen);
}

CSockAddr::~CSockAddr()
{
    if (m_pstSockAddr)
    {
        delete m_pstSockAddr;
    }
}

CSockAddr::operator struct sockaddr*()
{
    if (!m_pstSockAddr)
    {
        m_pstSockAddr = (struct sockaddr*)malloc(m_dwSockLen);
        //memset(m_pstSockAddr, 0, sizeof (struct sockaddr_in));
    }
    return (struct sockaddr*)m_pstSockAddr;
}

CSockAddr::operator const struct sockaddr*() const
{
    return (const struct sockaddr*)m_pstSockAddr;
}

CSockAddr& CSockAddr::operator =(const CSockAddr& roSockAddr)
{
    memmove((struct sockaddr*)(*this), (const struct sockaddr*)roSockAddr, m_dwSockLen);
}

void CSockAddr::SetSockAddr(const struct sockaddr* pstSockAddr)
{
    memmove((struct sockaddr*)(*this), pstSockAddr, m_dwSockLen);
}

socklen_t CSockAddr::GetSockLen() const
{
    return m_dwSockLen;
}


// CSockAddrIn
CSockAddrIn CSockAddrIn::m_oConvert;

CSockAddrIn::CSockAddrIn(const struct sockaddr* pstSockAddr)
: CSockAddr(pstSockAddr, sizeof(struct sockaddr_in))
{
    
}

CSockAddrIn::CSockAddrIn(const char* pAddr, int iPort)
: CSockAddr(NULL, sizeof(struct sockaddr_in))
{
    m_pstSockAddr = (struct sockaddr*)new struct sockaddr_in;
    CSockAddrIn::MakeSockAddrInStruct(m_pstSockAddr, pAddr, iPort);
}

void CSockAddrIn::SetSockAddrIn(const char* pAddr, int iPort)
{
    CSockAddrIn::MakeSockAddrInStruct((struct sockaddr*)(*this), pAddr, iPort);
}

void CSockAddrIn::MakeSockAddrInStruct(struct sockaddr* pstSockAddr, const char* pAddr, int iPort)
{
    ((struct sockaddr_in*)pstSockAddr)->sin_family = AF_INET;
    if (pAddr)
    {
        ((struct sockaddr_in*)pstSockAddr)->sin_addr.s_addr = inet_addr(pAddr);
    }
    else
    {
        ((struct sockaddr_in*)pstSockAddr)->sin_addr.s_addr = INADDR_ANY;
    }
    ((struct sockaddr_in*)pstSockAddr)->sin_port = htons(iPort);
    memset(((struct sockaddr_in*)pstSockAddr)->sin_zero, 0, sizeof (((struct sockaddr_in*)pstSockAddr)->sin_zero));
}

CSockAddrIn::operator struct sockaddr_in*()
{
    if (!m_pstSockAddr)
    {
        m_pstSockAddr = (struct sockaddr*)new struct sockaddr_in;
        //memset(m_pstSockAddr, 0, sizeof (struct sockaddr_in));
    }
    return (struct sockaddr_in*)m_pstSockAddr;
}

CSockAddrIn::operator const struct sockaddr_in*() const
{
    return (const struct sockaddr_in*)m_pstSockAddr;
}

char* CSockAddrIn::GetAddr()
{
    return m_pstSockAddr ? inet_ntoa(((struct sockaddr_in*)m_pstSockAddr)->sin_addr) : NULL;
}

int CSockAddrIn::GetPort()
{
    return m_pstSockAddr ? ntohs(((struct sockaddr_in*)m_pstSockAddr)->sin_port) : -1;
}

CSockAddrIn* CSockAddrIn::ConvertToSockAddrIn(const struct sockaddr* pstSockAddr)
{
    CSockAddrIn::m_oConvert.SetSockAddr(pstSockAddr);
    return &CSockAddrIn::m_oConvert;
}

CSockAddrIn* CSockAddrIn::ConvertToSockAddrIn(const char* pAddr, int iPort)
{
    CSockAddrIn::MakeSockAddrInStruct(CSockAddrIn::m_oConvert, pAddr, iPort);
    return &CSockAddrIn::m_oConvert;
}

// CSocketAddrLl
CSockAddrLl CSockAddrLl::m_oConvert;

CSockAddrLl::CSockAddrLl(const struct sockaddr* pstSockAddr)
: CSockAddr(pstSockAddr, sizeof(struct sockaddr_ll))
{
    
}

CSockAddrLl::CSockAddrLl(const char* pInterface, const char* pHardAddr)
: CSockAddr(NULL, sizeof(struct sockaddr_ll))
{
    m_pstSockAddr = (struct sockaddr*)new struct sockaddr_ll;
    CSockAddrLl::MakeSockAddrLlStruct(m_pstSockAddr, pInterface, pHardAddr);
}

void CSockAddrLl::SetSockAddrLl(const char* pInterface, const char* pHardAddr)
{
    CSockAddrLl::MakeSockAddrLlStruct((struct sockaddr*)(*this), pInterface, pHardAddr);
}

void CSockAddrLl::MakeSockAddrLlStruct(struct sockaddr* pstSockAddr, const char* pInterface, const char* pHardAddr)
{
    static int iTmpSck = socket(AF_INET, SOCK_DGRAM, 0);
    memset((struct sockaddr_ll*)pstSockAddr, 0, sizeof(struct sockaddr_ll));
    ((struct sockaddr_ll*)pstSockAddr)->sll_family = AF_PACKET;
    ((struct sockaddr_ll*)pstSockAddr)->sll_protocol = htons(ETH_P_ALL);
    ((struct sockaddr_ll*)pstSockAddr)->sll_halen = ETH_ALEN;
    
    struct ifreq ifr = {0};
    if (pInterface)
    {
        strcpy(ifr.ifr_name, pInterface);
        if (ioctl(iTmpSck, SIOCGIFINDEX, &ifr) < 0)
        {
            //LOG_POS("ioctl SIOCGIFINDEX err");
            return;
        }
        ((struct sockaddr_ll*)pstSockAddr)->sll_ifindex = ifr.ifr_ifindex;
    }
    
    /*
    memset(&ifr, 0, sizeof(ifr));
    if (pInterface)
    {
        strcpy(ifr.ifr_name, pInterface);
    }
    
    if (ioctl(iTmpSck, SIOCGIFHWADDR, &ifr) < 0)
    {
        LOG_POS("ioctl SIOCGIFHWADDR err");
        return;
    }
    
    memmove(((struct sockaddr_ll*)pstSockAddr)->sll_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
    */
    if (pHardAddr)
    {
        uint8_t* pAddr = ((struct sockaddr_ll*)pstSockAddr)->sll_addr;
        sscanf(pHardAddr, "%02x:%02x:%02x:%02x:%02x:%02x", (uint32_t*)&pAddr[0], (uint32_t*)&pAddr[1], (uint32_t*)&pAddr[2], (uint32_t*)&pAddr[3], (uint32_t*)&pAddr[4], (uint32_t*)&pAddr[5]);
    }
}

CSockAddrLl::operator struct sockaddr_ll*()
{
    if (NULL == m_pstSockAddr)
    {
        m_pstSockAddr = (struct sockaddr*)new struct sockaddr_ll;
        //memset(m_pstSockAddr, 0, sizeof (struct sockaddr_in));
    }
    return (struct sockaddr_ll*)m_pstSockAddr;
}

CSockAddrLl::operator const struct sockaddr_ll*() const
{
    return (const struct sockaddr_ll*)m_pstSockAddr;
}

char* CSockAddrLl::GetAddr()
{
    static char szAddr[128];
    const uint8_t* pAddr = ((const struct sockaddr_ll*)m_pstSockAddr)->sll_addr;
    snprintf(szAddr, sizeof(szAddr), "%02x:%02x:%02x:%02x:%02x:%02x", pAddr[0], pAddr[1], pAddr[2], pAddr[3], pAddr[4], pAddr[5]);
    return szAddr;
}

CSockAddrLl* CSockAddrLl::ConvertToSockAddrLl(const struct sockaddr* pstSockAddr)
{
    CSockAddrLl::m_oConvert.SetSockAddr(pstSockAddr);
    return &CSockAddrLl::m_oConvert;
}

CSockAddrLl* CSockAddrLl::ConvertToSockAddrLl(const char* pInterface, const char* pHardAddr)
{
    CSockAddrLl::MakeSockAddrLlStruct(CSockAddrLl::m_oConvert, pInterface, pHardAddr);
    return &CSockAddrLl::m_oConvert;
}


// CSocket

CSocket::CSocket(int iSock)
: m_iFd(iSock)
, m_pSockAddr(NULL)
{
    
}

CSocket::~CSocket()
{
    Close();
}

bool CSocket::Bind(const CSockAddr* pSockAddr)
{
    if (m_iFd < 0)
    {
        if (!CreateSocket())
        {
            return false;
        }
    }
    
    if (!m_pSockAddr)
    {
        return false;
    }
    *m_pSockAddr = *pSockAddr;
    
    
    return bind(m_iFd, *m_pSockAddr, m_pSockAddr->GetSockLen()) == 0;
}

CSockAddr* CSocket::GetSockAddr()
{
    return m_pSockAddr;
}

bool CSocket::SetSockOpt(int iLevel, int iOptName, const void* pOptVal, socklen_t uOptlen)
{
    if (m_iFd < 0)
    {
        errno = EBADF;
        return false;
    }
    return setsockopt(m_iFd, iLevel, iOptName, pOptVal, uOptlen) == 0;
}

bool CSocket::SetRecvTimeout(int iTimeout)
{
    struct timeval tv;
    tv.tv_sec = iTimeout / 1000;
    tv.tv_usec = (iTimeout % 1000) * 1000;
    return SetSockOpt(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (struct timeval));
}



bool CSocket::Close()
{
    if (close(m_iFd) != -1)
    {
        m_iFd = -1;
        return true;
    }
    else
    {
        return false;
    }
}

int CSocket::Send(const void* pBuf, int iSize)
{
    return send(m_iFd, pBuf, iSize, 0);
}

int CSocket::Recv(void* pBuf, int iSize)
{
    return recv(m_iFd, pBuf, iSize, 0);
}

int CSocket::SendTo(const void* pBuf, int iSize, const CSockAddr* pSockAddr)
{
    return sendto(m_iFd, pBuf, iSize, 0, *pSockAddr, pSockAddr->GetSockLen());
}

int CSocket::RecvFrom(void* pBuf, int iSize, CSockAddr* pSockAddr)
{
    socklen_t dwAddrSize = sizeof (struct sockaddr);
    return recvfrom(m_iFd, pBuf, iSize, 0, *pSockAddr, &dwAddrSize);
}

CSocket::operator int() const
{
    return m_iFd;
}



// CIpSocket

CIpSocket::CIpSocket(int iSock)
: CSocket(iSock)
{
    m_pSockAddr = new CSockAddrIn;
}

CIpSocket::~CIpSocket()
{
    Close();
}

bool CIpSocket::Bind(const char* pAddr, int iPort, bool bReuseAddr)
{
    if (m_iFd < 0)
    {
        if (!CreateSocket())
        {
            return false;
        }
    }
    int iOpt = bReuseAddr;
    if (!SetSockOpt(SOL_SOCKET, SO_REUSEADDR, &iOpt, sizeof(iOpt)))
    {
        return false;
    }
    return CSocket::Bind(CSockAddrIn::ConvertToSockAddrIn(pAddr, iPort));
}

// CUdpSocket

CUdpSocket::CUdpSocket(int iSock)
: CIpSocket(iSock)
//, m_iEpoll(-1)
{

}

bool CUdpSocket::CreateSocket()
{
    if (m_iFd > 0)
    {
        Close();
    }
    m_iFd = socket(PF_INET, SOCK_DGRAM, 0);
    return m_iFd > 0;
}


// CTcpSocket

CTcpSocket::CTcpSocket(int iSock)
: CIpSocket(iSock)
{
}

bool CTcpSocket::CreateSocket()
{
    if (m_iFd > 0)
    {
        Close();
    }
    m_iFd = socket(PF_INET, SOCK_STREAM, 0);
    return m_iFd > 0;
}

bool CTcpSocket::Connect(const CSockAddr* pSockAddr)
{
    if (m_iFd < 0)
    {
        if (!CreateSocket())
        {
            return false;
        }
    }
    *m_pSockAddr = *pSockAddr;
    return connect(m_iFd, *pSockAddr, pSockAddr->GetSockLen()) == 0;
}

bool CTcpSocket::Connect(const char* pAddr, int iPort)
{
    return Connect(CSockAddrIn::ConvertToSockAddrIn(pAddr, iPort));
}

bool CTcpSocket::Listen(int iBacklog)
{
    return listen(m_iFd, iBacklog) == 0;
}

bool CTcpSocket::Accept(CTcpSocket* pStreamSock)
{
    socklen_t uAddrSize = sizeof (struct sockaddr_in);
    pStreamSock->m_iFd = accept(m_iFd, *(pStreamSock->GetSockAddr()), &uAddrSize);
    return pStreamSock->m_iFd != -1;
}

bool CTcpSocket::SendEx(const void* pBuf, int iSize)
{
    int iRet;
    int iPos = 0;
    while (iSize > 0)
    {
        iRet = Send((uint8_t*)pBuf + iPos, iSize);
        if (iRet <= 0)
        {
            return false;
        }
        
        iPos += iRet;
        iSize -= iRet;
    }
    
    return true;
}

bool CTcpSocket::RecvEx(void* pBuf, int iSize)
{
    int iRet;
    int iPos = 0;
    while (iSize > 0)
    {
        iRet = Recv((uint8_t*)pBuf + iPos, iSize);
        if (iRet <= 0)
        {
            return false;
        }
        
        iPos += iRet;
        iSize -= iRet;
    }
    
    return true;
}

// CIpSocket

CEthSocket::CEthSocket(int iSock)
: CSocket(iSock)
{
    m_pSockAddr = new CSockAddrLl;
}

bool CEthSocket::CreateSocket()
{
    if (m_iFd > 0)
    {
        Close();
    }
    m_iFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    return m_iFd > 0;
}

bool CEthSocket::Bind(const char* pInterface)
{
    return CSocket::Bind(CSockAddrLl::ConvertToSockAddrLl(pInterface, NULL));
}

bool CEthSocket::SetPromisc(const char* pInterface, bool bOn)
{
    if (m_iFd < 0)
    {
        errno = EBADF;
        return false;
    }
    if (!pInterface)
    {
        return false;
    }
    struct ifreq ifr;
    strncpy(ifr.ifr_name, pInterface, sizeof(ifr.ifr_name));
    if (ioctl(m_iFd, SIOCGIFFLAGS, &ifr) < 0)
    {
        return false;
    }
    if (bOn)
    {
        ifr.ifr_flags |= IFF_PROMISC;
    }
    else
    {
        ifr.ifr_flags &= ~IFF_PROMISC;
    }
    if (ioctl(m_iFd, SIOCSIFFLAGS, &ifr) < 0)
    {
        return false;
    }
    return true;
}


