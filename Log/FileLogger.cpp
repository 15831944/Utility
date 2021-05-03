/* 
 * File:   FileLogger.cpp
 * Author: 0007989
 * 
 * Created on 2014年1月27日, 下午 3:59
 */

#include "FileLogger.h"
#include "../DateTime.h"
#include "LogMessage.h"
#include <stdarg.h>
#include <string.h>

namespace KGI_TW_Der_Utility
{   

FileLogger::FileLogger(const std::string& filename):m_Filename(filename) {
    m_FileID = NULL;    
    
}

FileLogger::FileLogger(const std::string& filename, const std::string& loggerName )
:ILogger(loggerName),m_Filename(filename){
    m_FileID = NULL;
}

FileLogger::~FileLogger() {
    Close();
}

bool FileLogger::Open(){
    if( m_Filename.size() <= 0 ) return false;
    
#ifdef _OS_LINUX_
    m_FileID = ::fopen( m_Filename.c_str(), "a+");    
#else // _WIN32
	m_FileID = ::_fsopen( m_Filename.c_str(), "a+", _SH_DENYWR);
#endif
   
    return m_FileID != NULL;
}

void FileLogger::Close(){   
    
    if( m_FileID != NULL )
    {
        ::fclose( m_FileID );
        m_FileID = NULL;
    }
}

long int FileLogger::GetSize() const{
    if( m_FileID != NULL )
        return ftell( m_FileID );
    else
        return -1;
}

void FileLogger::WriteLogString( const char* pMsg ){
    if( m_FileID == NULL )
        return;
    try
    {
        std::unique_lock<std::mutex> al(m_FileLock);
        
        //::fprintf( m_FileID, "%s", pMsg );
        ::fwrite( pMsg, strlen(pMsg), 1, m_FileID );
        ::fflush( m_FileID );     

    }
    catch( ... )
    {     
    }
}

void FileLogger::WriteBlock( const char* pBuffer, int length ){
    if( m_FileID == NULL )
        return;
    try
    {
        std::unique_lock<std::mutex> al(m_FileLock);
        
        //::fprintf( m_FileID, "%s", pMsg );
        ::fwrite( pBuffer, length, 1, m_FileID );
        ::fflush( m_FileID );     
    }
    catch( ... )
    {     
    }
}
}
