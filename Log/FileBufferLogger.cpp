/* 
 * File:   FileBufferLogger.cpp
 * Author: 0007989
 * 
 * Created on 2014年8月14日, 上午 11:41
 */

#include "FileBufferLogger.h"
#include "../DateTime.h"
#include "LogMessage.h"
#include <stdarg.h>
#include <string.h>

namespace KGI_TW_Der_Utility
{   

FileBufferLogger::FileBufferLogger(std::string filename):m_Filename(filename) {
    m_FileID = NULL;
    m_BufferSize = 10240;
    m_BufferPos = 0;
}

FileBufferLogger::FileBufferLogger(std::string filename, std::string loggerName )
    :ILogger(loggerName),m_Filename(filename){
    m_FileID = NULL;
}

FileBufferLogger::~FileBufferLogger() {
    Close();
}

bool FileBufferLogger::Open(){
    if( m_Filename.size() <= 0 ) return false;
    
#ifdef _OS_LINUX_
    m_FileID = ::fopen( m_Filename.c_str(), "a+");    
#else // _WIN32
	::fopen_s(&m_FileID, m_Filename.c_str(), "a+");
#endif
        
    InitBuffer( 10240 );
   
    return m_FileID != NULL;
}

void FileBufferLogger::Close(){   
    
    Flush();    
    
    if( m_FileID != NULL )
    {
        ::fclose( m_FileID );
        m_FileID = NULL;
    }
}

long int FileBufferLogger::GetSize(){
    if( m_FileID != NULL )
        return ftell( m_FileID );
    else
        return -1;
}

void FileBufferLogger::InitBuffer( int bufferSize ){
    m_BufferSize = bufferSize;
    m_Buffer.reset( new char[m_BufferSize] );
    memset( m_Buffer.get(), 0, sizeof(char) * m_BufferSize );
    m_BufferPos = 0;
}

void FileBufferLogger::WriteLogString( const char* pMsg ){
    if( m_FileID == NULL )
        return;
    try
    {   
        std::unique_lock<std::mutex> al(m_FileLock);
        
        int len = strlen( pMsg );
        if( m_BufferPos + len > m_BufferSize ){
            FlushBuffer();
        }
        if( len > m_BufferSize ){
            InitBuffer( len * 2 );
        }
        memcpy( m_Buffer.get() + m_BufferPos, pMsg, len );
        m_BufferPos += len;

    }
    catch( ... )
    {     
    }
}

void FileBufferLogger::Flush(){
    std::unique_lock<std::mutex> al(m_FileLock);
    
    FlushBuffer();
}

void FileBufferLogger::FlushBuffer(){
    // Flush buffer
    if( m_BufferPos > 0 ){
        ::fwrite( m_Buffer.get(), m_BufferPos, 1, m_FileID );
        ::fflush( m_FileID );
        m_BufferPos = 0;
    }
}

}

