/* 
 * File:   SocketBase.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月10日, 上午 8:13
 */

#include "SocketBase.h"
#include <string.h>
#include <errno.h>
#ifdef _OS_LINUX_
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#ifndef _CYGWIN_
#include <sys/epoll.h>
#endif

#else
#include <WinSock2.h>
#include <winsock.h>
#endif
//#include <inaddr.h>

#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <thread>

namespace ProgLab{ namespace Utility{ 

	SocketLib SocketBase::m_SocketLib;
 
SocketBase::SocketBase() : m_SocketID(-1){
    //m_LocalPort = 0;
    memset( &m_LocalIPInfo, 0, sizeof( m_LocalIPInfo ));
    //m_RemotePort = 0;
    memset( &m_RemoteIPInfo, 0, sizeof( m_RemoteIPInfo));
}

SocketBase::SocketBase(const SocketBase& orig) {
}

SocketBase::~SocketBase() {
    Disconnect();
}

bool SocketBase::GetRemoteInfo( int socketID, int *pIP, int &port ){
    
    // Get Remote infor
    struct sockaddr_in clientInfo;    
    memset( &clientInfo, 0, sizeof(clientInfo)); 
    socklen_t socklen = sizeof(clientInfo);
    int rc = ::getpeername( socketID, (sockaddr *)&clientInfo, &socklen);
    if( rc != 0 )
        return false;
    
    for( int i = 0; i < 4; ++i)
    {
        pIP[i] = (int)((clientInfo.sin_addr.s_addr & (0xFF << 8*i)) >> 8*i);
    }
    port = ntohs(clientInfo.sin_port);
    
    return true;
}

bool SocketBase::GetLocalInfo(  int socketID, int *pIP, int &port ){
    // Get Remote infor
    struct sockaddr_in clientInfo;    
    memset( &clientInfo, 0, sizeof(clientInfo)); 
    socklen_t socklen = sizeof(clientInfo);
    int rc = ::getsockname( socketID, (sockaddr *)&clientInfo, &socklen);
    if( rc != 0 )
        return false;
    
    for( int i = 0; i < 4; ++i)
    {
        pIP[i] = (int)((clientInfo.sin_addr.s_addr & (0xFF << 8*i)) >> 8*i);
    }
    port = ntohs(clientInfo.sin_port);
    
    return true;
}

void SocketBase::GetIP( int (&ip)[4]) const {
    for( int i = 0; i < 4; ++i )
    {
        ip[i] = m_LocalIPInfo.IP[i];
    }
}

void SocketBase::GetRemoteIP( int (&ip)[4] ) const{
    for( int i = 0; i < 4; ++i )
    {
        ip[i] = m_RemoteIPInfo.IP[i];
    }
}

void SocketBase::Disconnect(){
    if( m_SocketID > 0 )
    {
#ifdef _OS_LINUX_
        shutdown( m_SocketID, SHUT_RDWR ); // 2015/03/09 Justin 要呼叫shutdown()函式，才能讓GatewayClient的ReceiveClientData()函式中的rcev()收到結束的訊息
#else
        shutdown( m_SocketID, SD_BOTH ); // 2015/03/09 Justin 要呼叫shutdown()函式，才能讓GatewayClient的ReceiveClientData()函式中的rcev()收到結束的訊息
#endif
        close( m_SocketID );
        m_SocketID = 0;
    }
}

bool SocketBase::ConvertIP( const char* pIP, in_addr_t& result ){
    
#ifdef _OS_LINUX_
	struct sockaddr_in svrAddr;
	memset(&svrAddr, 0, sizeof(svrAddr));
    if( SOCKET_CONVERT_IP(AF_INET, pIP, &(svrAddr.sin_addr)) != 1 )
#else
	IN_ADDR svrAddr;
	memset(&svrAddr, 0, sizeof(svrAddr));
    if( SOCKET_CONVERT_IP(AF_INET, (PCSTR)pIP, &svrAddr) != 1 )
#endif
        return false;
    else{
#ifdef _OS_LINUX_
        result = svrAddr.sin_addr.s_addr;
#else
		result = svrAddr.S_un.S_addr;
#endif
        return true;
    }    
}

int SocketBase::Bind( int fd, const char* pIP, int port ){
    struct sockaddr_in svrAddr;
    
    memset( &svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    if( pIP == NULL )
        svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);    
    else{
        //svrAddr.sin_addr.s_addr = inet_addr( pIP );  // 為配合Windows版本，改用inet_pton()      
        if( ConvertIP(pIP, svrAddr.sin_addr.s_addr) == false )
            return -1;
    }
    svrAddr.sin_port = htons(port);
    
    int rc = ::bind( fd, (struct sockaddr*)&svrAddr, sizeof(svrAddr));
    if( rc != 0 ) 
    {        
        char pErrMsg[500];
        //char *pErrMsg = strerror( errno );
#ifdef _OS_LINUX_
        strerror_r( errno, pErrMsg, 500 );
#else // _WIN32
	strerror_s(pErrMsg, 500, errno);
#endif
        printf( "%s\r\n", pErrMsg );
        //delete[] pErrMsg;
        
    }
    return rc;
}

int SocketBase::Bind( const std::string& ip, int port ){
    return SocketBase::Bind( m_SocketID, ip.c_str(), port );
}


void SocketBase::InitIPInfo(){
    SocketBase::GetLocalInfo( m_SocketID, m_LocalIPInfo.IP, m_LocalIPInfo.Port );
    SocketBase::GetRemoteInfo( m_SocketID, m_RemoteIPInfo.IP, m_RemoteIPInfo.Port );
}

void SocketBase::SetTcpNodelay(int fd) {
    int enable = 1;
#ifdef _OS_LINUX_
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
#else
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));
#endif
}

bool SocketBase::ReadyToWrite( int socketID, int timeoutMS ){
    fd_set rfds;
    struct timeval tv;
    int retval;
    
    FD_ZERO( &rfds );
    FD_SET( socketID, &rfds );
    
    tv.tv_sec = timeoutMS / 1000;
    tv.tv_usec = (timeoutMS % 1000) * 1000;
    
    retval = select( 1, NULL, &rfds, NULL, &tv );
    return retval > 0;
}

bool SocketBase::GetIPAddress( const std::string& interfaceName, std::string& ip ){
#ifdef _OS_LINUX_
    struct ifaddrs* ipinfo;
    memset( &ipinfo, 0, sizeof(ipinfo) );
    if( getifaddrs( &ipinfo) == 0 ){
        // success
        struct ifaddrs* crtIPInfo = ipinfo;
        while( crtIPInfo != NULL ){
            if( crtIPInfo->ifa_addr->sa_family == AF_INET ){
                
                std::string crtIPName(crtIPInfo->ifa_name);
                if( crtIPName == interfaceName ){
                    struct sockaddr_in *pAddr = (struct sockaddr_in *)crtIPInfo->ifa_addr;                
                    printf( "IPV4 Name=%s,IP=%s\r\n", crtIPInfo->ifa_name, inet_ntoa(pAddr->sin_addr) );
                    ip = inet_ntoa(pAddr->sin_addr);
                    
                    return true;
                }
                
            }
            crtIPInfo = crtIPInfo->ifa_next;
        }
    }
#else
	// TODO : implement windows version
#endif
    
    return false;
}

#ifdef _OS_LINUX_
bool SocketBase::SetNonBlocking( int fd ){
    int flags, s;
 
    flags = fcntl (fd, F_GETFL, 0);
    if (flags == -1)
    {        
        return false;
    }
 
    flags |= O_NONBLOCK;
    s = fcntl (fd, F_SETFL, flags);
    if (s == -1)
    {    
        return false;
    }
 
    return true;
}
#else
bool SocketBase::SetNonBlocking(int fd){
	u_long flags, s;

	flags = ioctlsocket(fd, FIONBIO, 0);
	if (flags == -1)
	{
		return false;
	}

	flags = 1; // 在windows裏設定flags不為0時就是啟用non-blocking
	s = ioctlsocket(fd, FIONBIO, &flags);
	if (s == -1)
	{
		return false;
	}

	return true;
}
#endif

#if defined(_OS_LINUX_) && !defined(_CYGWIN_)
EPollSocket::EPollSocket() : m_EPollFD(-1){}

EPollSocket::~EPollSocket(){
    
}

bool EPollSocket::Init(){
    if( m_SocketID < 0 ){
        m_SocketID = socket( AF_INET, SOCK_STREAM, 0 );
        if( m_SocketID <= 0 )
            return false;
    }
    
    // 設為non-blocking
    if( SocketBase::SetNonBlocking( m_SocketID ) == false )
        return false;
    
    // Init EPOLL
    m_EPollFD = epoll_create1 (0);
    if (m_EPollFD == -1){      
        return false;
    }
    
    return false;
}

void EPollSocket::UnInit(){
    if( m_EPollFD > 0 ){
        close( m_EPollFD );
        m_EPollFD = -1;
    }
    
    if( m_SocketID > 0 ){
        close( m_SocketID );
        m_SocketID = 0;
    }
}

EPollSocketClient::EPollSocketClient( int socketID ) : m_ConnectPort(0) {
    m_SocketID = socketID;
}
    
EPollSocketClient::~EPollSocketClient(){}

void EPollSocketClient::Connect( const char* pIP, int port, int localPort ){
    
    m_IP = pIP;
    m_ConnectPort = port;
    
    if( Init() == false ){
        throw std::runtime_error( "EPollSocketClient::Init() failed" );
    }
    
    // if local port is specified, then bind 
    if( localPort > 0 ){
        if( SocketBase::Bind( pIP, localPort ) != 0 )
            throw std::runtime_error( "EPollSocketClient::Connect() failed" );
    }
    
    std::thread tConnectThread( std::bind(&EPollSocketClient::ConnectThread, this ) );
    tConnectThread.detach();
}

void EPollSocketClient::ConnectThread(){
    
    struct sockaddr_in svrAddr;
    
    memset( &svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    if( m_IP.size() <= 0 )
        svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);    
    else
        svrAddr.sin_addr.s_addr = inet_addr( m_IP.c_str() );
    svrAddr.sin_port = htons(m_ConnectPort);
    
    int rc = connect( m_SocketID, (struct sockaddr*)&svrAddr, sizeof(svrAddr));
    if( rc == 0 ){ // connected
        if( m_OnConnected != NULL ){
            m_OnConnected(m_SocketID, true, "");
        }
    }else if( errno == EINPROGRESS ){
        struct epoll_event newPeerConnectionEvent;        
        struct epoll_event processableEvents;
        unsigned int numEvents = -1;
        
        newPeerConnectionEvent.data.fd = m_SocketID;
        newPeerConnectionEvent.events = EPOLLOUT | EPOLLIN | EPOLLERR;

        if (epoll_ctl (m_EPollFD, EPOLL_CTL_ADD, m_SocketID, &newPeerConnectionEvent) == -1){            
            if( m_OnConnected != NULL ){
                m_OnConnected( m_SocketID, false, "Could not add the socket FD to the epoll FD list. Aborting!" );
            }
        }

        numEvents = epoll_wait (m_EPollFD, &processableEvents, 1, -1);
        
        if (numEvents < 0){           
            if( m_OnConnected != NULL ){
                m_OnConnected( m_SocketID, false, "Serious error in epoll setup: epoll_wait () returned < 0 status!" );
            }
        }else{
            SocketBase::GetLocalInfo( m_SocketID, m_LocalIPInfo.IP, m_LocalIPInfo.Port );
            SocketBase::GetRemoteInfo( m_SocketID, m_RemoteIPInfo.IP, m_RemoteIPInfo.Port );
    
            if( m_OnConnected != NULL ){
                m_OnConnected( m_SocketID, true, "" );
            }
            
            // start reading thread
            std::thread tReceive( std::bind( &EPollSocketClient::ReceiveThread, this) );
            tReceive.detach();
        }
    }
}

void EPollSocketClient::ReceiveThread(){
    
}
#endif


}