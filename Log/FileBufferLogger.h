/* 
 * File:   FileBufferLogger.h
 * Author: 0007989
 *
 * Created on 2014年8月14日, 上午 11:41
 */

#ifndef UTILITY_FILEBUFFERLOGGER_H
#define	UTILITY_FILEBUFFERLOGGER_H

#include "ILogger.h"

#include <mutex>
#include <string>
#include <cstdio>
#include <memory>

//class RollFileLogger;

namespace KGI_TW_Der_Utility
{   

class FileBufferLogger : public ILogger {
    //friend class RollFileLogger;
public:
    FileBufferLogger(std::string filename );    
    FileBufferLogger(std::string filename, std::string loggerName );    
    virtual ~FileBufferLogger();
    
    bool Open();
    void Close();
    void Flush();
    
    void SetMaxLogLevel( ILogger::LogLevel logLevel ){ ILogger::SetMaxLogLevel( logLevel ); }
    
    long int GetSize();    

    virtual void WriteLogString( const char* pMsg );
protected:
    std::string m_Filename;
    FILE *m_FileID;
    mutable std::mutex m_FileLock;
    
    std::shared_ptr<char> m_Buffer;
    int m_BufferSize, m_BufferPos;
    
private:
    void FlushBuffer();
    void InitBuffer( int bufferSize );
};

}

#endif	/* UTILITY_FILEBUFFERLOGGER_H */

