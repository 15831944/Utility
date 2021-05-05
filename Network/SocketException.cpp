/* 
 * File:   SocketException.cpp
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 * 
 * Created on 2015年7月28日, 上午 8:50
 */

#include "SocketException.h"

namespace ProgLab{ namespace Utility{

SocketException::SocketException(const char *pMsg ){ 
    if( pMsg != NULL )
        m_Message.assign( pMsg );
}

SocketException::SocketException(const SocketException& orig)  : std::exception(orig){    
    m_Message = orig.m_Message;
}

SocketException::~SocketException() throw() {
}

SocketException& SocketException::operator=( const SocketException& orig ){
    if( this != &orig )
    {
        m_Message = orig.m_Message;
    }
    return *this;
}

SocketWriteException::SocketWriteException( int willSentBytes, int sentBytes, const char* pMsg ) : SocketException(pMsg), m_WillSentBytes(willSentBytes), m_SentBytes(sentBytes){
    
}
    
SocketWriteException::SocketWriteException( const SocketWriteException& orig ):SocketException(orig){
    *this = orig;
}

SocketWriteException& SocketWriteException::operator=( const SocketWriteException& orig ){
    if( this != &orig ){
        *(reinterpret_cast<SocketException*>( this )) = reinterpret_cast<const SocketException&>( orig );
        m_WillSentBytes = orig.m_WillSentBytes;
        m_SentBytes = orig.m_SentBytes;
    }
    return *this;
}

}
