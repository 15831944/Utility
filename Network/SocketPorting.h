/* 
 * File:   SocketPorting.h
 * Author: 0007989
 *
 * Created on 2014年8月6日, 下午 5:13
 */

#ifndef UTILITY_SOCKETPORTING_H
#define	UTILITY_SOCKETPORTING_H

#ifdef _OS_LINUX_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#define SOCK_OPT_VALUE(x) x
#define SOCKET_CONVERT_IP inet_pton
#elif defined(_WIN32)

#include <WinSock2.h>
#include <ws2ipdef.h> // for udp
#include <Ws2tcpip.h>
#include <winsock.h>
#define SOCK_OPT_VALUE(x) reinterpret_cast<char*>( x )
#define close closesocket
typedef int socklen_t;
#define SOCKET_CONVERT_IP InetPton
typedef ULONG in_addr_t;
#else

#endif 

#endif	/* UTILITY_SOCKETPORTING_H */

