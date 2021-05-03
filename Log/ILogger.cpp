/* 
 * File:   ILogger.cpp
 * Author: 0007989
 * 
 * Created on 2014年1月27日, 下午 2:42
 */

#include "ILogger.h"

#include "../DateTime.h"
#include "LogSystem.h"
#include "../StringUtil.h"
#include "LogMessage.h"
#include <thread>

namespace KGI_TW_Der_Utility
{
   
const char ILogger::g_LogLevel[][10] = {"Debug ","Debug3","Debug2","Debug1","Warn  ","Info  ","Error ","Fatal "};

ILogger::ILogger() {
    m_pDateStr = NULL;
    m_MaxLogLevel = LogLevel::Level_Debug;
    
    InitDateStr();
}

ILogger::ILogger(const std::string& loggerName):m_LoggerName(loggerName){
    m_pDateStr = NULL;
    m_MaxLogLevel = LogLevel::Level_Debug;
    
    InitDateStr();
}

ILogger::~ILogger() {
    if( m_pDateStr != NULL )
        delete[] m_pDateStr;

    m_pDateStr = NULL;
}

const std::string& ILogger::GetName() const{
    return m_LoggerName;
}

void ILogger::SetName(const std::string &name){
    m_LoggerName = name;
}

void ILogger::InitDateStr(){    
    DateTime dtNow;
    int defaultSize = 0;
    m_pDateStr = StringUtil::StringFormat( defaultSize, "%04d-%02d-%02d", dtNow.GetYear(), dtNow.GetMonth(), dtNow.GetDay() );    
}

ILogger::LogLevel ILogger::GetMaxLogLevel() const{
    return m_MaxLogLevel;
}

void ILogger::SetMaxLogLevel( LogLevel logLevel ){
    m_MaxLogLevel = logLevel;
}

void ILogger::WriteLog( LogLevel logLevel, const std::string& msg ){    
    
    if( logLevel >= m_MaxLogLevel )
    {    
        const char* pMsg = MakeLogMessage( ILogger::g_LogLevel[logLevel], msg.c_str() );
        if( pMsg != NULL ){
            WriteLogString( pMsg );
            delete[] pMsg;
        }
    }    
}

void ILogger::WriteLog( LogLevel logLevel, const char *pFmt, ... ){
    if( m_MaxLogLevel <= logLevel ){        
        va_list ap;
        va_start(ap,pFmt);
        WriteLogImpl( logLevel, pFmt, ap );       
        va_end(ap);
    }
}

void ILogger::WriteLogImpl( LogLevel logLevel, const char *pFmt, va_list &arg ){
    // 產生要的 LogMessage
    std::string logMsg;    
    StringUtil::StringFormatEx( logMsg, pFmt, arg, 100 );
    if( logMsg.empty() == false ){
        WriteLog( logLevel, logMsg );
    }
}

const char* ILogger::MakeLogMessage( const char* pLevelString, const char* pMsg ) const{
    int defaultSize = 0;
    
    DateTime dtNow;        
    char* pTime = StringUtil::StringFormat( defaultSize, "%02d:%02d:%02d.%03d.%03d", dtNow.GetHour(), dtNow.GetMinute(), dtNow.GetSecond(), dtNow.GetMillisecond(), dtNow.GetMicrosecond() );
    if( pTime == NULL )
        return NULL;
    defaultSize = 0;
    char* pResult = StringUtil::StringFormat( defaultSize, "%s-%s [%s] [%06d] [%s] %s\r\n", m_pDateStr, pTime, pLevelString, CurrentThread::GetThreadID(),GetName().c_str(), pMsg );
    delete[] pTime;
    return pResult;
    
}

void ILogger::MakeLogMessage( std::string& result, const char* pLevelString, const char* pMsg, int defaultSize ) const{
    DateTime dtNow; 
    if( defaultSize > 0 )
        result.reserve( defaultSize );
    StringTemplateUtil::Format( result, "%s-%02d:%02d:%02d.%03d.%03d [%s] [%06d] [%s] %s\r\n", m_pDateStr,
            dtNow.GetHour(), dtNow.GetMinute(), dtNow.GetSecond(), dtNow.GetMillisecond(), dtNow.GetMicrosecond(),
            pLevelString, CurrentThread::GetThreadID(),GetName().c_str(), pMsg, defaultSize
            );
}


}

