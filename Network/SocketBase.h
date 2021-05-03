/* 
 * File:   SocketBase.h
 * Author: 0007989
 *
 * Created on 2014年4月10日, 上午 8:13
 */

#ifndef UTILITY_SOCKETBASE_H
#define	UTILITY_SOCKETBASE_H

#include <string>

#include "SocketPorting.h"
#include "../MacroRecipe.h"
#include "../StringTemplateUtil.h"

#include <functional>
//#include <arpa/inet.h>

// 使用pragma comment可以避免使用此SocketBase的人忘了加入引用Ws2_32.lib
#if defined(_WIN64) || defined(_WIN32)
#pragma comment( lib, "Ws2_32.lib" )
#endif

namespace KGI_TW_Der_Utility
{ 

	class SocketLib {
	public:
		SocketLib() {
#if defined(_WIN64) || defined(_WIN32)
			WSADATA data;
			WORD wVersionRequested = MAKEWORD(2, 2);
			int err = WSAStartup( wVersionRequested, &data );
			if (err != 0) {
				throw std::runtime_error("Init Windows Socket lib failed");
			}
			else
				printf("Windows Socket lib initialized\r\n");
#endif
		}
		virtual ~SocketLib() {
#if defined(_WIN64) || defined(_WIN32)
			WSACleanup();
			printf("Windows Socket lib uninitialized\r\n");
#endif
		}
	};

	


    
/// Socket的連線資訊
struct SocketIPInfo{
    
    int IP[4];     
    int Port;
    
    /// 格式化成 xxx.xxx.xxx.xxx:port    
    std::string ToString() const{
        std::string info;
        StringTemplateUtil::Format( info, "%d.%d.%d.%d:%d", IP[0], IP[1], IP[2], IP[3], Port );
        return info;
    }
};

class SocketBase {
public:
    SocketBase();
    SocketBase(const SocketBase& orig);
    virtual ~SocketBase();
    
    void Disconnect();
    
    int GetSocketID() const { return m_SocketID; }
    
    int GetLocalPort() const { return m_LocalIPInfo.Port; }
    int GetRemotePort() const { return m_RemoteIPInfo.Port; }
    const int *GetLocalIP() const { return m_LocalIPInfo.IP; }
    const int *GetRemoteIP() const { return m_RemoteIPInfo.IP; }
    
    void GetIP( int (&ip)[4] ) const;
    void GetRemoteIP( int (&ip)[4] ) const;
    
    /// \param pIP return the remote ip address. It should be int[4]]
    static bool GetRemoteInfo( int socketID, int *pIP, int &port );
    /// \param pIP return the local ip address. It should be int[4]]
    static bool GetLocalInfo( int socketID, int *pIP, int &port );
    static void SetTcpNodelay(int fd);  
    static bool SetNonBlocking( int fd );
    /// \param pIP 如果是空字串，則會bind到目前的所有網路介面上
    /// \return rc of socket, 0 means success
    static int Bind( int fd, const char* pIP, int port );
    /// 使用inet_pton()函式來轉換字串IP成為socket用的ip。主要是為了要跟window版本通用    
    static bool ConvertIP( const char* pIP, in_addr_t& result );
    /// \brief 等待一段時間看指定的socket是否可以進行傳送
    /// \return false=>buffer滿了無法傳送
    static bool ReadyToWrite( int socketID, int timeoutMS );
    /// \brief 取得指定介面的IP位址
    /// \param interfaceName 要取得IP的介面名稱，例如eth0
    /// \param ip 傳回所取得的IP位址
    /// \return 無法取得時傳回false
    static bool GetIPAddress( const std::string& interfaceName, std::string& ip );
protected:
	static SocketLib m_SocketLib;
    volatile int m_SocketID;             //!< socket ID
//    int m_LocalIP[4];
//    int m_LocalPort;
    SocketIPInfo m_LocalIPInfo;
//    int m_RemoteIP[4];
//    int m_RemotePort;
    SocketIPInfo m_RemoteIPInfo;
    static const int InvalidSocketID = -1;
    
protected:
    /// \param ip 如果是空字串，則會bind到目前的所有網路介面上
    /// \return rc of socket, 0 means success
    int Bind( const std::string& ip, int port );
    /// \brief 取得本地端IP及遠地端IP的資訊
    void InitIPInfo();
    
};
#if defined(_OS_LINUX_) && !defined(_CYGWIN_)
class EPollSocket : public SocketBase{
public:
    EPollSocket();
    virtual ~EPollSocket();
    
    bool Init();
    void UnInit();    
    
protected:
    int m_EPollFD;    
    /// param - int => socketID of connected client
    std::function<void(int, bool, const std::string&)> m_OnConnected;
    std::function<void(int,const std::string&)> m_OnError;
    std::function<void(int, const char* pBuffer, int size )> m_OnReceive;
    
protected:
    void StartRead();
    void StopRead();
};

class EPollSocketClient : public EPollSocket{
public:
    EPollSocketClient( int socketID = -1 );
    virtual ~EPollSocketClient();
    
    void Connect( const char* pIP, int port, int myPort = 0 );
protected:
    void ConnectThread();
    void ReceiveThread();
    
protected:
    std::string m_IP;
    int m_ConnectPort;
};

class EPollHelper{
public:
    EPollHelper();
    virtual ~EPollHelper();
protected:
    int m_EPollFD;    
};
#endif
}
#endif	/* UTILITY_SOCKETBASE_H */

