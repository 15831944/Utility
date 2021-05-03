/* 
 * File:   LogAgent.cpp
 * Author: 0007989
 * 
 * Created on 2015年1月15日, 下午 4:56
 */

#include "LogAgent.h"
#include "ILogger.h"
#include "LogMessage.h"
#include <thread>
#include "../StringUtil.h"

namespace KGI_TW_Der_Utility {

void LogAgent::Start(){
    std::thread tLogThread( std::bind(&LogAgent::WriteLogThread,this) );
    tLogThread.detach();
}

void LogAgent::Stop(){
    m_EventWriteThreadAborted.Reset();
    
    m_Queue.Abort();
    
    if( m_EventWriteThreadAborted.Wait(5000) == false ){
        // Timeout, 
    }
    
    m_Queue.Flush( std::bind(&LogAgent::WriteOut,this, std::placeholders::_1) );
}

void LogAgent::WriteMessage( ILogger::LogLevel level, const char* pMsg ){
    // check log level
    if( m_pLogger != NULL ){
        if( m_pLogger->GetMaxLogLevel() > level )
            return;
    }
    else{
        if( m_DefaultLogger.GetMaxLogLevel() > level )
            return;
    }    
    
    std::shared_ptr<LogAgentTask> task( new LogAgentTask() );
    task->m_Level = level;    
    task->m_Msg = pMsg;    
    task->m_ThreadID = CurrentThread::GetThreadID();

    m_Queue.Push( task );    
}

void LogAgent::Write( ILogger::LogLevel level, const char* pFmt, ... ){    
    // check log level
    if( m_pLogger != NULL ){
        if( m_pLogger->GetMaxLogLevel() > level )
            return;
    }
    else{
        if( m_DefaultLogger.GetMaxLogLevel() > level )
            return;
    }
    
    va_list ap;
    va_start(ap,pFmt);
    int defaultSize = 0;
    char *pResult = StringUtil::StringFormatImpl( defaultSize, pFmt, ap );
    WriteMessage( level, (const char*)pResult );
    delete[] pResult;
    va_end(ap);
    
}

void LogAgent::Write( ILogger::LogLevel level, const char* pFmt, va_list& ap ){
    // check log level
    if( m_pLogger != NULL ){
        if( m_pLogger->GetMaxLogLevel() > level )
            return;
    }
    else{
        if( m_DefaultLogger.GetMaxLogLevel() > level )
            return;
    }
    
    int defaultSize = 0;
    char *pResult = StringUtil::StringFormatImpl( defaultSize, pFmt, ap );
    WriteMessage( level, (const char*)pResult );
    delete[] pResult;            
}

void LogAgent::WriteOut( const std::shared_ptr<LogAgentTask>& task ){
    LogMessage lm;
    if( task != NULL ){
        lm.Format( "%04d-%02d-%02d-%02d:%02d:%02d.%03d.%03d [%s] [%06d] [%s] %s\r\n",
                    task->m_Now.GetYear(), task->m_Now.GetMonth(), task->m_Now.GetDay(), 
                    task->m_Now.GetHour(), task->m_Now.GetMinute(), task->m_Now.GetSecond(), task->m_Now.GetMillisecond(), task->m_Now.GetMicrosecond(),
                    ILogger::g_LogLevel[task->m_Level], task->m_ThreadID, m_pLogger == NULL ? "" : m_pLogger->GetName().c_str(), 
                    task->m_Msg.c_str() );

        if( m_pLogger != NULL ){                    
            m_pLogger->WriteLogString( lm.GetMessage() );
        }else{
            m_DefaultLogger.WriteLogString( lm.GetMessage() );
        }
    }    
}

void LogAgent::WriteLogThread(){    
    while(true){
        try{
            std::shared_ptr<LogAgentTask> task = m_Queue.Pop();
            WriteOut( task );
        }catch(...){
            break;
        }
    }
    
    m_EventWriteThreadAborted.Set(true);
}

}

