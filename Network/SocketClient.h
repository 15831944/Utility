/* 
 * File:   SocketClient.h
 * Author: 0007989
 *
 * Created on 2014年4月14日, 上午 9:57
 */

#ifndef UTILITY_SOCKETCLIENT_H
#define	UTILITY_SOCKETCLIENT_H

#ifdef _OS_LINUX_
#include <netinet/in.h>
#endif
#include <string>
#include "SocketBase.h"
#include "SocketChannel.h"

namespace KGI_TW_Der_Utility
{

class SocketClient : public SocketChannel {
public:
    SocketClient();
    SocketClient(const SocketClient& orig);
    virtual ~SocketClient();
    
    bool Connect( const std::string& ip, int port, int localPort = 0 );  
    void SetNoDelay();
protected:

};

}
#endif	/* UTILITY_SOCKETCLIENT_H */

