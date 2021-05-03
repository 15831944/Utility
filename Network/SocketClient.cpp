/* 
 * File:   SocketClient.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月14日, 上午 9:57
 */

#include "SocketClient.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef _OS_LINUX_
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#endif

namespace KGI_TW_Der_Utility
{

SocketClient::SocketClient():SocketChannel(-1) {    
}

SocketClient::SocketClient(const SocketClient& orig) : SocketChannel(orig) {
}

SocketClient::~SocketClient() {   
}

bool SocketClient::Connect( const std::string& ip, int port, int localPort ){
    m_SocketID = socket( AF_INET, SOCK_STREAM, 0 );
    if( m_SocketID < 0 )
        return false;
    
    // if local port is specified, then bind 
    if( localPort > 0 ){
        if( Bind( ip, localPort ) != 0 )
            return false;
    }
    
    struct sockaddr_in svrAddr;
    
    memset( &svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    if( ip.size() <= 0 )
        svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);    
    else{
        //svrAddr.sin_addr.s_addr = inet_addr( ip.c_str() ); // 為配合Windows版本，改用inet_pton()
        if( ConvertIP(ip.c_str(), svrAddr.sin_addr.s_addr) == false )
            return false;
    }
    svrAddr.sin_port = htons(port);    
    
    int rc = connect( m_SocketID, (struct sockaddr*)&svrAddr, sizeof(svrAddr));
    if( rc != 0 ) 
    {
        int errNo = errno;        
        //char *pErrMsg = strerror( errno );
	char pErrMsg[500];
#ifdef _OS_LINUX_
        strerror_r(errno, pErrMsg, 500);
#else // _WIN32
	strerror_s(pErrMsg, 500, errno);
#endif
        printf( "%s\r\n", pErrMsg );        
        return false;
    }
    
    InitIPInfo();
    
    return true;
}

void SocketClient::SetNoDelay(){
    SocketBase::SetTcpNodelay( m_SocketID );
}
}