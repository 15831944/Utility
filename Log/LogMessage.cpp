/* 
 * File:   LogMessage.cpp
 * Author: 0007989
 * 
 * Created on 2014年3月11日, 上午 11:33
 */

#include "LogMessage.h"
#include "../StringUtil.h"

#include <stdarg.h>
#include <string.h>
#include <assert.h>


namespace KGI_TW_Der_Utility
{    

LogMessage::LogMessage(int length):m_pMessage(NULL){
    SetLength( length );
}

LogMessage::LogMessage(const LogMessage& orig):m_pMessage(NULL) {
    SetLength( orig.m_Length );
    memcpy( m_pMessage, orig.m_pMessage, m_Length );
}

LogMessage::~LogMessage() {
    delete[] m_pMessage;
}

int LogMessage::Format( const char *pFmt, ... ){
    va_list ap, check;
    va_start(ap,pFmt);
    va_copy(check,ap);  // copy ap to check
    int length = m_Length; // copy, otherwise it will be modified
    char *pResult = StringUtil::StringFormatImpl( m_pMessage, length, pFmt, check );
    if( pResult == NULL ){
        // need to extend memory
        SetLength( length );
        pResult = StringUtil::StringFormatImpl( m_pMessage, length, pFmt, ap );
        //assert( pResult != NULL );
    }
    m_pMessage = pResult;     
    return length;
}

void LogMessage::SetLength( int length ){
    m_Length = length;
    delete[] m_pMessage;
    m_pMessage = NULL;
    if( length > 0 ){
        m_pMessage = new char[m_Length];
        ::memset( m_pMessage, 0, m_Length );
    }
}

const char *LogMessage::GetMessageImpl(){
    return m_pMessage;
}

}
