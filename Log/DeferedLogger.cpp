/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DeferedLogger.cpp
 * Author: Justin Lin
 * 
 * Created on 2017年6月2日, 上午 10:50
 */

#include <thread>

#include "../DateTime.h"
#include "DeferedLogger.h"
#include "ILogData.h"
#include "../StringTemplateUtil.h"
#include "../StringUtil.h"

namespace KGI_TW_Der_Utility{

DeferedLogger::DeferedLogger() {
    std::thread tWriteLog( std::bind( &DeferedLogger::DeferedWriteLogThread, this) );
    tWriteLog.detach();
}

//DeferedLogger::DeferedLogger(const DeferedLogger& orig) {
//}

DeferedLogger::~DeferedLogger() {
    LogDataQueue_.Abort(); // Stop thread
}

void DeferedLogger::WriteLog( LogLevel logLevel, const std::string& msg ){
    if( m_MaxLogLevel <= logLevel ){
        std::shared_ptr<StringLogData> logData = std::make_shared<StringLogData>( msg );              
        if( logData ){
            WriteLog( logLevel, logData );
        }
    }
}

void DeferedLogger::WriteLog( LogLevel logLevel, const char *pFmt, ... ){
    if( m_MaxLogLevel <= logLevel ){        
        va_list ap;
        va_start(ap,pFmt);
        WriteLogImpl( logLevel, pFmt, ap );       
        va_end(ap);
    }
}

void DeferedLogger::WriteLogImpl( LogLevel logLevel, const char *pFmt, va_list &arg ){
    if( m_MaxLogLevel <= logLevel ){
        std::string logMsg;
        StringUtil::StringFormatEx( logMsg, pFmt, arg );

        WriteLog( logLevel, logMsg );
    }
}

void DeferedLogger::WriteLog( LogLevel logLevel, std::shared_ptr<ILogData> logData ){
    DateTime now;
    std::string stampStr;
    StringTemplateUtil::Format( stampStr, "%04d%02d%02d-%02d:%02d:%02d.%03d.%03d", now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond(), 
            now.GetMillisecond(), now.GetMicrosecond() );
    logData->SetDateTimeStr( stampStr );
    logData->SetLogLevelStr( ILogger::g_LogLevel[logLevel] );
    
    LogDataQueue_.Push( logData );
}

void DeferedLogger::DeferedWriteLogThread(){
    while(true){
        try{
            auto logData = LogDataQueue_.Pop();
            if( logData != NULL ){                
                this->WriteLogString( logData->ToString().c_str() );
            }
        }catch( BQThreadAbort& abort ){
            break;
        }catch(...){
            break;
        }
    }
}

}