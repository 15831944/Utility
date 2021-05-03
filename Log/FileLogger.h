/* 
 * File:   FileLogger.h
 * Author: 0007989
 *
 * Created on 2014年1月27日, 下午 3:59
 */

#ifndef UTILITY_FILELOGGER_H
#define	UTILITY_FILELOGGER_H

#include "ILogger.h"
#include <mutex>

#include <string>
#include <cstdio>

//class RollFileLogger;

namespace KGI_TW_Der_Utility
{   

class FileLogger : public ILogger {
    //friend class RollFileLogger;
public:
    FileLogger(const std::string& filename );    
    FileLogger(const std::string& filename, const std::string& loggerName );    
    virtual ~FileLogger();
    
    virtual bool Open();
    virtual void Close();
    
    void SetMaxLogLevel( ILogger::LogLevel logLevel ){ ILogger::SetMaxLogLevel( logLevel ); }
    
    long int GetSize() const;
    
    //virtual void WriteLog( LogLevel logLevel, std::string msg );
    //virtual void WriteLog( LogLevel logLevel, const char *pFmt, ... );
    virtual void WriteLogString( const char* pMsg );
    void WriteBlock( const char* pBuffer, int length );
protected:
    std::string m_Filename;
    FILE *m_FileID;
    mutable std::mutex m_FileLock;
};

}

#endif	/* UTILITY_FILELOGGER_H */

