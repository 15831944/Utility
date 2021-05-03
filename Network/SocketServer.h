/* 
 * File:   SocketServer.h
 * Author: 0007989
 *
 * Created on 2014年1月23日, 下午 4:59
 */

#ifndef UTILITY_SOCKETSERVER_H
#define	UTILITY_SOCKETSERVER_H

//#include <netinet/in.h>

//#include "../MT/ThreadWrapper.h"
#include "SocketServerListener.h"
#include "../MT/ManualResetEvent.h"
#include <string>
#include "SocketChannel.h"
#include <memory>
#include <functional>

using namespace KGI_TW_Der_Utility;

namespace KGI_TW_Der_Utility
{
   
///
///  SocketServer 
/// 
///  Communication with TCP.    
class SocketServer : public SocketChannel {
public:
    /// StatusCode
    enum StatusCode
    {
        SC_OK,          //!< Status OK
        SC_ERROR,       //!< Status ERROR
        SC_CreateSocketFaile,
    };
        
    enum class AcceptModeEnum{
        None,
        AcceptByListener,
        AcceptByFunction,
    };
public:
    /// Constructor
    /// \param port the port for waiting client to connect in
    SocketServer(int port);    
    
    SocketServer(const std::string &ip, int port);  
    /// Destructor
    virtual ~SocketServer();
    
    /// Start server
    /// \param pListener for receiving client connected in event
    /// \return \sa StatusCode
    StatusCode Start( SocketServerListener *pListener );
    // Start server without derived from SocketServerListener
    // \param function for accept client connected. The type of function is void(*)(SocketChannel *)
    // \return \sa StatusCode
    StatusCode Start( std::function<void(SocketChannel*)> acceptFunc );        
    StatusCode Start( std::function<void(SocketChannel*)> acceptFunc, std::function<SocketChannel*(int)> createSocketChannel  );
    /// Stop server    
    void Stop();
    /// Get server port
    /// \return port number
    int GetPort(){ return m_Port; }
    /// Get ip address from given struct sockaddr_in
    /// \param pClientInfo the ip address information that returned from \sa accept()
    /// \param cilentIP for receiving the IP address
    /// \as accept()
    static void GetIPAddress( struct sockaddr_in *pClientInfo, int (&clientIP)[4] );
private:
    std::string m_IP;
    int m_Port;                             //!< server port        
    AcceptModeEnum m_AcceptMode;
    SocketServerListener *m_pListener;      //!< 
    std::function<void(SocketChannel*)> m_AcceptFunc;   //!< the callback function for accepting client
    ManualResetEvent m_AcceptThreadStopped;    
    std::function<SocketChannel*(int)> m_CreateSocketChannel;   //!< 給外部指定如何產生SocketChannel
private:
    StatusCode StartImpl();    
    void AcceptThread( SocketServer* pMySelf );
    SocketChannel* DefaultCreateSocketChannel( int socketID );

};

}

#endif	/* UTILITY_SOCKETSERVER_H */

