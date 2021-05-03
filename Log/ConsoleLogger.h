/* 
 * File:   ConsoleLogger.h
 * Author: 0007989
 *
 * Created on 2014年1月27日, 下午 2:46
 */

#ifndef UTILITY_CONSOLELOGGER_H
#define	UTILITY_CONSOLELOGGER_H

#include "ILogger.h"
#include <mutex>
#include "LogMessage.h"

namespace KGI_TW_Der_Utility
{   


class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    ConsoleLogger( const char *pName );
    virtual ~ConsoleLogger();
    
    //void WriteLog( LogLevel logLevel, const std::string& msg );
    virtual void WriteLogString( const char* pMsg );
private:
    mutable std::mutex m_Lock;
    LogMessage m_LogMsg;

};

}

#endif	/* UTILITY_CONSOLELOGGER_H */

