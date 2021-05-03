/* 
 * File:   SocketServerListener.h
 * Author: 0007989
 *
 * Created on 2014年1月27日, 上午 11:29
 */

#ifndef UTILITY_SOCKETSERVERLISTENER_H
#define	UTILITY_SOCKETSERVERLISTENER_H

//class SocketChannel;
#include "SocketChannel.h"

namespace KGI_TW_Der_Utility
{       

class SocketServerListener {
public:    
    virtual ~SocketServerListener();
    
    virtual void OnClientConnected( SocketChannel *pSocketChannel ) = 0;
protected:
    SocketServerListener();    

};

}

#endif	/* UTILITY_SOCKETSERVERLISTENER_H */

