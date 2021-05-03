/* 
 * File:   RollFileLogger.cpp
 * Author: 0007989
 * 
 * Created on 2014年5月27日, 下午 5:40
 */

#include "RollFileLogger.h"
#ifdef _OS_LINUX_
#include <dirent.h>
#else // _WIN32
#include <Windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LogMessage.h"

#include <stdarg.h>
#include "../StringUtil.h"

namespace KGI_TW_Der_Utility{          

RollFileLogger::RollFileLogger(  const char *pFilenamePattern,int maxSizeMB, const char *pLoggerName ) : 
    _BaseFileLogType_( std::string(pFilenamePattern), std::string("") ),m_MaxSizeMB(maxSizeMB)
{
    m_LastIndex = 0;
    m_FilenamePattern = pFilenamePattern;
    
    m_LastIndex = GetLastIndex(m_FilenamePattern);    
}

RollFileLogger::~RollFileLogger() {
}

//void RollFileLogger::WriteLog( LogLevel logLevel, const char *pFmt, ... ){
//    va_list ap;
//    va_start(ap,pFmt);
//    FileLogger::WriteLogImpl( logLevel, pFmt, ap );
//    va_end(ap);
//}

void RollFileLogger::WriteLogString( const char* pMsg ){
    try
    {
        std::unique_lock<std::mutex> al( m_FileOperation );
        
        _BaseFileLogType_::WriteLogString( pMsg ); // use base class to write log

        // Check the file size
        long int fileSize = GetSize();            
        if( fileSize > m_MaxSizeMB * RollFileLogger::MBSize )
        {
            // Switch file
            Close();
            
            // Rename
            LogMessage lm;
            m_LastIndex++;
            lm.Format( "%s.%d", m_FilenamePattern.c_str(), m_LastIndex );
            int rcRename = rename( m_FilenamePattern.c_str(), lm.GetMessage() );
            if( rcRename != 0 )
            {
                // rename failed
            }

            Open();
        }
    }
    catch( ... )
    {    
    }
}

#ifdef _OS_LINUX_
int RollFileLogger::GetLastIndex( const std::string& filePath ){
    DIR* d;
    struct dirent* dir;
    int lastIndex = 0;
    
    std::string rootPath, filename;
    if( ParsePath( filePath, rootPath, filename ) == false )
        return 0;
    if( filename.size() <= 0 )
        return 0;
    
    filename += '.';
    
    if( rootPath.size() <= 0 ){
        rootPath = ".";
    }        
    
    d = opendir( rootPath.c_str()); 
    if( d != NULL )
    {    
        while( (dir = readdir(d)) != NULL )
        {
            std::string fileInPath = dir->d_name;
            if( strstr( dir->d_name, filename.c_str() )  != NULL  &&
                    fileInPath.size() > filename.size() )
            {
                std::string postFix( fileInPath.c_str(), filename.size(), fileInPath.size() - filename.size() );
                int crtIndex = atoi( postFix.c_str() );
                if( crtIndex > lastIndex )
                    lastIndex = crtIndex;
            }
        }
        closedir(d);
        return lastIndex;
    }
    else
        return 0;        
}
#else
int RollFileLogger::GetLastIndex(const std::string& filePath){
	
	std::string rootPath, filename;
	if (ParsePath(filePath, rootPath, filename) == false)
		return 0;
	if (filename.size() <= 0)
		return 0;

	filename += '*';
        
        if( rootPath.size() <= 0 ){
            rootPath = ".";
        } 
        
        int lastIndex = 0;

	std::string searchPath = rootPath + "\\" + filename;

	WIN32_FIND_DATA findFile;
	HANDLE hFind = FindFirstFile((LPCSTR)searchPath.c_str(), &findFile);
	while (hFind != INVALID_HANDLE_VALUE){
		std::string foundName( (const char*)findFile.cFileName);
		std::size_t pos = foundName.rfind(".");
		if (pos != std::string::npos){
			std::string indexValue = foundName.substr(pos, foundName.size() - pos);
			int crtIndex = atoi(indexValue.c_str());
			if (crtIndex > lastIndex)
                            lastIndex = crtIndex;
		}
		FindNextFile(hFind, &findFile);
	}
	return lastIndex;
}
#endif

bool RollFileLogger::ParsePath( const std::string& path, std::string& rootPath, std::string& filename ){
#ifdef _OS_LINUX_
    std::string::size_type splitPos = path.rfind( '/' );
#else // _WIN32
	std::string::size_type splitPos = path.rfind( '\\' );
#endif
	if( splitPos == std::string::npos )
	{
		rootPath = "";
		filename = path;
	}
	else
	{
		rootPath.assign(path.c_str(), splitPos + 1);
		int filenameLength = path.size() - splitPos - 1;
		filename.assign(path.c_str(), splitPos + 1, filenameLength);
	}
    return true;
}


}