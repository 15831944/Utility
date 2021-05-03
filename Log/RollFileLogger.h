/* 
 * File:   RollFileLogger.h
 * Author: 0007989
 *
 * Created on 2014年5月27日, 下午 5:40
 */

#define _RollFileLogger_EX_

#ifndef UTILITY_ROLLFILELOGGER_H
#define	UTILITY_ROLLFILELOGGER_H

#include "ILogger.h"

#include <string>
#include <cstdio>
#include <memory>

#include "FileLogger.h"
#include "FileBufferLogger.h"
#include <mutex>
#include "../MT/DataDispatcher.h"

namespace KGI_TW_Der_Utility{   

typedef FileLogger _BaseFileLogType_;

class RollFileLogger : public _BaseFileLogType_
{
public:
    RollFileLogger( const char *pFilenamePattern, int maxSizeMB, const char *pLoggerName = NULL );    
    virtual ~RollFileLogger();    

    virtual void WriteLogString( const char* pMsg );    
    
    static bool ParsePath( const std::string& path, std::string& rootPath, std::string& filename );    
    static int GetLastIndex(const std::string& filePath );   
    
public:
    static const int MBSize = 1024 * 1024;
private:    
    std::string m_FilenamePattern;
    int m_MaxSizeMB;
    int m_LastIndex;        //!< the last index of rolled log file
    mutable std::mutex m_FileOperation;    
//private:
    //void WriteLogImpl( LogLevel logLevel, const std::string& msg );    
    
};

}
#endif	/* UTILITY_ROLLFILELOGGER_H */

