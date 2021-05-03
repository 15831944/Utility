/* 
 * File:   SocketChannel.h
 * Author: 0007989
 *
 * Created on 2014年1月27日, 上午 11:03
 */

#ifndef UTILITY_SOCKETCHANNEL_H
#define	UTILITY_SOCKETCHANNEL_H

#include <mutex>

#include "SocketBase.h"

namespace KGI_TW_Der_Utility
{   

///
///  SocketChannel
///  Use socket TCP protocol to communicate
///  
///  After client connect to a SocketServer, SocketServer will create a SocketChannel and notify
///  SocketServerListener to receive the new connected client.
///  \sa SocketServer,SocketChannel,SocketServerListener
class SocketChannel : public SocketBase {
public:
    SocketChannel( int socketID );
    SocketChannel(const SocketChannel& orig);
    virtual ~SocketChannel();
    
    void Close(){ Disconnect(); }
    
    // \param timeoutUS in us unit, 0 => disable timeout
    // \return 0 => success
    int SetWriteTimeout( int timeoutUS );
    PROP_GETSET_BYVALUE( int, WriteTryCounts, m_WriteTryCounts );
    // \param timeoutUS in us unit, 0 => disable timeout
    // \return 0 => success
    int SetReadTimeout( int timeoutUS );
    
    virtual int Read( char *pBuffer, int bufferSize, int startPos );
    virtual int Write( const char *pBuffer, int bufferSize );
    /// \brief 配合設定 recv timieout的模式來讀取資料
    /// 需先呼叫 SetReadTimeout() 設定接收資料的timeout時間
    /// \param pIsTimeout 傳回此次接收結果是否為timieout。如果為timeout時，傳回的接收資料數一般都是-1
    /// \return 傳回已接收到的資料數
    virtual int Read( char *pBuffer, int bufferSize, int startPos, bool* pIsTimeout );
protected:        
    mutable std::mutex m_Object;            //!< lock for synchronize socket access
    int m_WriteTryCounts;                   //!< the maximum times for retry send() 
protected:
    /// Initialize the information of socket, such as remote ip and port
    void InitInfo();
};

}

#endif	/* UTILITY_SOCKETCHANNEL_H */

