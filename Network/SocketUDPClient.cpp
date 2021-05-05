/* 
 * File:   SocketUDPClient.cpp
 * Author: 0007989
 * 
 * Created on 2014年7月10日, 下午 5:39
 */

#include "SocketUDPClient.h"

#include <cstdlib>
#include <string.h>
//#ifdef _OS_LINUX_
//#include <netinet/in.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#define SOCK_OPT_VALUE(x) x
//#else
//#include <WinSock2.h>
//#define SOCK_OPT_VALUE(x) reinterpret_cast<char*>( x )
//#endif

namespace ProgLab{ namespace Utility{

SocketUDPClient::SocketUDPClient() {
}

SocketUDPClient::SocketUDPClient(const SocketUDPClient& orig) {
}

SocketUDPClient::~SocketUDPClient() {
}

bool SocketUDPClient::Connect( const char* pIP, int port, const char* pMyIP ){    
    
    
    m_SocketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if( m_SocketID < 0 )
        return false;
    
    int reuseflag = 1; /* socket option flag */
    
    // set reuse port to on to allow multiple binds per host
	if ((setsockopt(m_SocketID, SOL_SOCKET, SO_REUSEADDR, SOCK_OPT_VALUE( &reuseflag ), sizeof (reuseflag))) < 0) {
        //perror("Reusing ADDR failed");
        return false;
    }

    sockaddr_in mc_addr; /* socket address structure */
    // set up destination address 
    //construct a multicast address structure 
    memset(&mc_addr, 0, sizeof (mc_addr));
    mc_addr.sin_family = AF_INET;    
//    if( ConvertIP(pMyIP, mc_addr.sin_addr.s_addr) == false ) // mc_addr.sin_addr.s_addr =inet_addr(pMyIP); 
//        return false;
    mc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    mc_addr.sin_port = htons(port);
    
     // bind to multicast address to socket
    if ((bind(m_SocketID, (struct sockaddr *) &mc_addr, sizeof (mc_addr))) < 0) {
        //perror("bind() failed");
        return false;
    }

	ip_mreq mc_group; /* MC request structure */
	memset(&mc_group, 0, sizeof(mc_group));
        
    if( ConvertIP( pIP, mc_group.imr_multiaddr.s_addr ) != true )       // mc_group.imr_multiaddr.s_addr = inet_addr(pIP);
        return false;    
    if( ConvertIP( pMyIP, mc_group.imr_interface.s_addr ) != true )     // mc_group.imr_interface.s_addr =inet_addr(pMyIP);// htonl(INADDR_ANY);
        return false;
    // send an ADD MEMBERSHIP message via setsockopt
	if ((setsockopt(m_SocketID, IPPROTO_IP, IP_ADD_MEMBERSHIP, SOCK_OPT_VALUE(&mc_group), sizeof (mc_group))) < 0) {
        //perror("setsockopt() failed");
        //exit(1);
        return false;
    }  
    
    return true;
}

int SocketUDPClient::Receive( char* pBuffer, int bufferSize ){
    
    sockaddr_in from_addr; /* packet source */  
    memset(&from_addr, 0, sizeof (from_addr));
//    from_addr.sin_family = AF_INET;
//    from_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//    from_addr.sin_port = htons(0);
    
    socklen_t from_len = sizeof (from_addr); /* source addr length */    
    int bytes = recvfrom(m_SocketID, pBuffer, bufferSize, 0, (struct sockaddr*) &from_addr, &from_len); 
    return bytes;
}

void SocketUDPClient::Close(){
    close( m_SocketID );
    m_SocketID = 0;
}

}