/* 
 * File:   SocketChannel.cpp
 * Author: 0007989
 * 
 * Created on 2014年1月27日, 上午 11:03
 */

#include "SocketChannel.h"


#include <sys/types.h>
#include <string.h>
#ifdef _OS_LINUX_
#include <error.h>
#endif
#include <errno.h>
#include "SocketBase.h"
#include "SocketException.h"

namespace KGI_TW_Der_Utility{   

SocketChannel::SocketChannel(int socketID ):m_WriteTryCounts(3){
    m_SocketID = socketID;    
    
    InitInfo();
}

SocketChannel::SocketChannel(const SocketChannel& orig) {
    m_SocketID = orig.m_SocketID;
    m_WriteTryCounts = orig.m_WriteTryCounts;
    
    InitInfo();
}

SocketChannel::~SocketChannel() {
    //Close(); // 2015/04/01 Justin 因為在SocketBase中的解構子已有執行Disconnect()，所以就不用重覆執行
}

void SocketChannel::InitInfo(){
    if( m_SocketID > 0 ){
        int flag = 1;
        int rc = ::setsockopt(m_SocketID, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
        if( rc < 0 )
        {
            int errCode = errno;
            fprintf( stderr, "setsocketopt() failed with RC(%d) errno(%d)\r\n", rc, errCode );
        }
    }
    
    InitIPInfo();      
}

int SocketChannel::SetWriteTimeout( int timeoutUS ){
    struct timeval timeout;
    timeout.tv_sec = timeoutUS / 1000000;
    timeout.tv_usec = timeoutUS % 1000000; // % 1000000 * 1000 = % 1000
    socklen_t len = sizeof(timeout);  
    return setsockopt(m_SocketID, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&timeout), len);    
}

int SocketChannel::SetReadTimeout( int timeoutUS ){
    struct timeval timeout;
    timeout.tv_sec = timeoutUS / 1000000;
    timeout.tv_usec = timeoutUS % 1000000; // % 1000000 * 1000 = % 1000
    socklen_t len = sizeof(timeout);  
    return setsockopt(m_SocketID, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout), len);    
}

int SocketChannel::Read( char *pBuffer, int bufferSize, int startPos ){
    if( m_SocketID <= 0 )
        return 0;
    
    size_t readBytes = ::recv( m_SocketID, pBuffer, bufferSize - startPos, 0);
    
    return readBytes;
}

int SocketChannel::Read( char *pBuffer, int bufferSize, int startPos, bool* pIsTimeout ){
    if( m_SocketID <= 0 ){
        *pIsTimeout = false;
        return 0;
    }
    
    int readBytes = 0;
    while(true){    
        readBytes = ::recv( m_SocketID, pBuffer, bufferSize - startPos, 0 );
        int errorCode = errno;
        if( readBytes == -1 ){
            switch( errorCode ){
                case EAGAIN: // aka EWOULDBLOCK                
                    *pIsTimeout = true;
                    return readBytes;
                case EINTR: // continue to read
                    break;
                default:  // continue to read
                    break;
            }
        }else{
            *pIsTimeout = false;
            return readBytes;
        }
    }        
}

int SocketChannel::Write( const char *pBuffer, int bufferSize ){    
    if( m_SocketID <= 0 )
        return 0;
    
    int totalSent = 0;     
    try
    {        
        std::unique_lock<std::mutex> al(m_Object);
        const char *pSentBuffer = pBuffer;
        bool isExit = false;
        while(!isExit)
        {
            // http://stackoverflow.com/questions/108183/how-to-prevent-sigpipes-or-handle-them-properly
            // http://stackoverflow.com/questions/18935446/program-received-signal-sigpipe-broken-pipe
            // 2015/03/23 Justin
            // 可能在要傳送資料時，對方的socket斷線，這時會引發SIGPIPE的錯誤，造成程式中斷。
            // 在send()時加上MSG_NOSIGNAL，可以避免程式中斷
            // 2015/08/14 Justin
            // 當傳送的buffer滿了時，第一次會收到errno==11的值，這時可以再傳送一次，但如果還是滿的話，就會收到errno==9或88的訊息。
            // 所以要重傳三次的方式可能不行，要改用select()的方式來等待buffer清空
#ifdef _OS_LINUX_
            int sentBytes = ::send( m_SocketID, pSentBuffer, bufferSize - totalSent, MSG_NOSIGNAL );
#else
            int sentBytes = ::send( m_SocketID, pSentBuffer, bufferSize - totalSent, 0 );
#endif            
            if( sentBytes < 0 ){
                int sendErrorCode = errno;
                switch( sendErrorCode ){
                    case ENOBUFS:
                    case EAGAIN: //EWOULDBLOCK:
                        if( ReadyToWrite( m_SocketID, 5000 ) == false )
                            isExit = true;
                        break;
                    default:
                        isExit = true;
                }                
                
                sentBytes = 0; // 因為timeout而且未傳送出任何資料
            }else{
                // Send remained data 
                totalSent += sentBytes;
                if( totalSent >= bufferSize )
                    isExit = true; // 已傳送完畢
                else
                    pSentBuffer = pBuffer + totalSent; // sent remained buffers
            }            
        }
    }catch( SocketWriteException exp ){
        throw;
    }catch( SocketException exp ){
        throw;
    }catch( ... ){
        //m_SocketID = 0; // Disconnected
    }    
    
    return totalSent;
}
//
//int SocketChannel::Write( const char *pBuffer, int bufferSize ){    
//    if( m_SocketID <= 0 )
//        return 0;
//    
//    int totalSent = 0; 
//    int tryCount = m_WriteTryCounts;
//    try
//    {        
//        std::unique_lock<std::mutex> al(m_Object);
//        const char *pSentBuffer = pBuffer;
//        bool isExit = false;
//        while(!isExit)
//        {
//            // http://stackoverflow.com/questions/108183/how-to-prevent-sigpipes-or-handle-them-properly
//            // http://stackoverflow.com/questions/18935446/program-received-signal-sigpipe-broken-pipe
//            // 2015/03/23 Justin
//            // 可能在要傳送資料時，對方的socket斷線，這時會引發SIGPIPE的錯誤，造成程式中斷。
//            // 在send()時加上MSG_NOSIGNAL，可以避免程式中斷
//#ifdef _OS_LINUX_
//            int sentBytes = ::send( m_SocketID, pSentBuffer, bufferSize - totalSent, MSG_NOSIGNAL );
//#else
//            int sentBytes = ::send( m_SocketID, pSentBuffer, bufferSize - totalSent, 0 );
//#endif            
//            if( sentBytes < 0 ){
//                int sendErrorCode = errno;
//                if( sendErrorCode != ENOBUFS && sendErrorCode != EAGAIN && sendErrorCode != EWOULDBLOCK ){
//                    //m_SocketID = 0; // some error
//                    isExit = true;                    
//                }
//                
//                sentBytes = 0; // 因為timeout而且未傳送出任何資料
//            }
//            
//            if( !isExit ){
//                // Send remained data 
//                totalSent += sentBytes;
//                if( totalSent >= bufferSize )
//                    isExit = true; // 已傳送完畢
//                else{
//                    // Check retry count
//                    tryCount--;
//                    if( tryCount <= 0 ){                
//                        throw SocketWriteException( bufferSize, totalSent, "SocketChannel::Write() failed" );                
//                    }
//
//                    pSentBuffer = pBuffer + totalSent; // sent remained buffers
//                }
//            }
//        }
//    }catch( SocketWriteException exp ){
//        throw;
//    }catch( SocketException exp ){
//        throw;
//    }catch( ... ){
//        //m_SocketID = 0; // Disconnected
//    }    
//    
//    return totalSent;
//}
}