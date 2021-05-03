/* 
 * File:   LoggerWrapper.h
 * Author: 0007989
 *
 * Created on 2015年1月15日, 下午 4:54
 */

#ifndef UTILITY_LOGGERWRAPPER_H
#define	UTILITY_LOGGERWRAPPER_H

#include "LogSystem.h"

namespace KGI_TW_Der_Utility {

/// \brief Simulate ILogger with LogSystem
class LoggerWrapper : public ILogger{
public:
    LoggerWrapper(){}
    virtual ~LoggerWrapper(){}
    
    virtual LogLevel GetMaxLogLevel(){ return LogSystem::GetInstance().GetLogLevel(); }
    
    virtual void WriteLog( LogLevel logLevel, std::string msg ){
        LogSystem::GetInstance().WriteLogEx( logLevel, msg.c_str() );
    }

    virtual void WriteLogString( const char* pMsg ){
        LogSystem::GetInstance().WriteLogString( pMsg );
    }
};


}
#endif	/* UTILITY_LOGGERWRAPPER_H */

