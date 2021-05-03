/* 
 * File:   SocketServer.cpp
 * Author: 0007989
 * 
 * Created on 2014年1月23日, 下午 4:59
 */

#include "SocketServer.h"
#include "SocketServerListener.h"
#include "SocketChannel.h"
#include "AsyncSocketChannel.h"

//#include <sys/socket.h>
#include <errno.h>
//#include <netinet/in.h>
//#include <unistd.h>
//#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <memory>

#include "../CommonException.h"

namespace KGI_TW_Der_Utility
{
   

SocketServer::SocketServer(int port):SocketChannel(-1),m_Port(port),m_AcceptMode(AcceptModeEnum::None) {
    m_IP = "";    
    
    m_pListener = NULL;
}

SocketServer::SocketServer(const std::string &ip, int port):SocketChannel(-1),m_Port(port),m_AcceptMode(AcceptModeEnum::None){
    m_IP = ip;    
    
    m_pListener = NULL;
}  

SocketServer::~SocketServer() {
}

SocketServer::StatusCode SocketServer::Start(SocketServerListener *pListener){
    
    m_pListener = pListener;
    m_AcceptMode = AcceptModeEnum::AcceptByListener;
    
    return StartImpl();    
}

SocketServer::StatusCode SocketServer::StartImpl(){
    m_SocketID = socket( AF_INET, SOCK_STREAM, 0 );
//    int i = errno;
//    printf( "CreateSocket failed, with errno=%d\r\n", errno );
    if( m_SocketID <= 0 )
        return SC_CreateSocketFaile;
    
    SocketBase::SetTcpNodelay( m_SocketID );
    
    
    int rc = Bind( m_IP, m_Port );
    if( rc != 0 ){
        SocketBase::Disconnect();
        return SC_ERROR; 
    }
    
    rc = ::listen(m_SocketID,5);
    if( rc != 0 ){
        SocketBase::Disconnect();
        return SC_ERROR;   
    }        
    
    std::thread tAccept( &SocketServer::AcceptThread, this, this );
    tAccept.detach();
    
    return SC_OK;
}

SocketServer::StatusCode SocketServer::Start( std::function<void(SocketChannel*)> acceptFunc ){
    if( m_SocketID > 0 ){
        return SC_ERROR; // server already started.
    }
    m_AcceptFunc = acceptFunc;
    m_AcceptMode = AcceptModeEnum::AcceptByFunction;
    
    return StartImpl();
}

SocketServer::StatusCode SocketServer::Start( std::function<void(SocketChannel*)> acceptFunc, std::function<SocketChannel*(int)> createSocketChannel ){
    if( m_SocketID > 0 ){
        return SC_ERROR; // server already started.
    }
    
    m_CreateSocketChannel = createSocketChannel;
    
    m_AcceptFunc = acceptFunc;
    m_AcceptMode = AcceptModeEnum::AcceptByFunction;
    
    return StartImpl();
}

void SocketServer::Stop(){
    
    if( m_SocketID > 0 )
    {
        m_AcceptThreadStopped.Reset();
        int rc = 0;
#ifdef _OS_LINUX_
        rc = shutdown( m_SocketID, SHUT_RDWR); // 通知AcceptThread()離開
        printf( "SocketServer::Stop() shutdown=%d\r\n", rc );
#else
		shutdown(m_SocketID, SD_BOTH); // 通知AcceptThread()離開
#endif
        rc = close( m_SocketID);
        m_SocketID = 0;  // could stop the accept thread
        
        if( m_AcceptThreadStopped.Wait( 5000 ) == false ){
            std::cerr << "SocketServer::Stop() wait AcceptThread() stop timeout" << std::endl;
        }
        
        m_AcceptMode = AcceptModeEnum::None;
#if (__cplusplus >= 201103L) || (_MSC_VER >= 1800) // 因為在VS2013版中，它的__cplusplus仍是199711L
		m_pListener = nullptr;
		m_AcceptFunc = nullptr;
#else
		m_pListener = NULL;
		m_AcceptFunc = NULL;
#endif
    }
}


void SocketServer::AcceptThread(SocketServer* pServer ){
    
    struct sockaddr_in clientInfo;    
    memset( &clientInfo, 0, sizeof(clientInfo));   
    socklen_t clientInfoSize = sizeof(clientInfo);
    
    try
    {    
        while(true)
        {            
            volatile int socketID = pServer->m_SocketID;
            if( socketID <= 0 )
                break;
            
            int clientSocketID = ::accept( socketID, (struct sockaddr*)&clientInfo, &clientInfoSize );
            if( clientSocketID <= 0 )
            {
                //std::cout << "[SocketServer] accepted client failed,SocketID(" << socketID << ")" << std::endl;
                // socket closed
                break;
            }
            else
            {
                std::cout << "[SocketServer] client connected. SocketID=" << clientSocketID << std::endl;
              
                SocketChannel *pNewChannel = NULL;
                if( m_CreateSocketChannel == NULL )
                    pNewChannel = new SocketChannel( clientSocketID );
                else
                    pNewChannel = m_CreateSocketChannel( clientSocketID );
                switch( m_AcceptMode ){
                    case AcceptModeEnum::AcceptByListener:
                        if( pServer->m_pListener != NULL ){                            
                            pServer->m_pListener->OnClientConnected( pNewChannel );
                        }
                        break;
                    case AcceptModeEnum::AcceptByFunction:
                        if( pServer->m_AcceptFunc != NULL ){
                            pServer->m_AcceptFunc( pNewChannel );
                        }
                        break;
                    default:
                        delete pNewChannel;
                }
                
            }
        }
    }
    catch(...)
    {
        throw CommonException( __LINE__, __FILE__, "[StocketServer::AcceptThread()] Exception ");
    }
    
    std::cout << "[SocketServer] accepte thread stoped" << std::endl;
    
    m_AcceptThreadStopped.Set(true);
}

void SocketServer::GetIPAddress( struct sockaddr_in *pClientInfo, int (&clientIP)[4] ){
    
    for( int i = 0; i < 4; ++i)
    {
        clientIP[i] = (int)((pClientInfo->sin_addr.s_addr & (0xFF << 8*i)) >> 8*i);
    }
}

SocketChannel* SocketServer::DefaultCreateSocketChannel( int socketID ){
    return new SocketChannel( socketID );
}
}