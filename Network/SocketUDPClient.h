/* 
 * File:   SocketUDPClient.h
 * Author: 0007989
 *
 * Created on 2014年7月10日, 下午 5:39
 */

#ifndef UTILITY_SOCKETUDPCLIENT_H
#define	UTILITY_SOCKETUDPCLIENT_H

#include "SocketBase.h"

namespace KGI_TW_Der_Utility{

class SocketUDPClient : public SocketBase{
public:
    SocketUDPClient();
    SocketUDPClient(const SocketUDPClient& orig);
    virtual ~SocketUDPClient();
    
    /// \param pIP multicase ip
    /// \param pMyIP my ip
    bool Connect( const char* pIP, int port, const char* pMyIP );
    void Close();
    
    int Receive( char* pBuffer, int bufferSize );
private:

};

}
#endif	/* UTILITY_SOCKETUDPCLIENT_H */

