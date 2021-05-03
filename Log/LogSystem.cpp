/* 
 * File:   LogSystem.cpp
 * Author: 0007989
 * 
 * Created on 2014年1月27日, 下午 2:51
 */

#include "LogSystem.h"
#include <cstddef>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <thread>
#ifdef _OS_LINUX_
#include <unistd.h>
#ifndef _CYGWIN_
#if __cplusplus < 201103L
#include <sys/syscall.h>
#endif
#endif
#include <sys/types.h>
#include <bits/shared_ptr.h>
#endif
#include "../StringUtil.h"
#include "LogMessage.h"
#include "../Performance/StopWatch.h"
//#include "../MT/CurrentThread.h"

namespace KGI_TW_Der_Utility
{  
    
int LogSystem::m_LoggerID = 0;
LogSystem *LogSystem::m_pInstance = NULL;
std::mutex LogSystem::m_InstanceLocker;
std::shared_ptr<LogSystem> m_LogSystemInstance;

LogSystem::LogSystem() : m_LogLevel(ILogger::Level_Info) {
    m_WriteLogThread = std::thread( std::bind(&LogSystem::WriteLogThread,this) );
}

LogSystem::~LogSystem() {
    m_Queue.Abort();
    
    m_WriteLogThread.join();
    
    printf( "LogSystem::Queue Size=%d\r\n", m_Queue.Size() );
}

int LogSystem::Count() const{
    
    int count = 0;
    {
        std::unique_lock<decltype(m_LoggerLocker)> al( m_LoggerLocker );    
        count = m_Loggers.size();    
    }
    return count;
}

void LogSystem::Terminate(){
    // 改由shared_ptr來回收
//    std::unique_lock<std::mutex> al( m_InstanceLocker );
//    if( m_pInstance != NULL )
//    {
//        delete m_pInstance;
//        m_pInstance = NULL;
//    }
}

LogSystem &LogSystem::GetInstance()
{    
    // 2015/02/12 Justin 改為先檢臺再lock，以便節省執行時間
    if( m_pInstance == NULL ){
        std::lock_guard<decltype(m_InstanceLocker)> al( m_InstanceLocker );
        if( m_pInstance == NULL ){
            m_pInstance = new LogSystem();
            m_LogSystemInstance.reset( m_pInstance ); // 放到global的shared_ptr中管理，當系統終止時可以自動呼叫LogSystem的destructor
        }
    }
    
    return *m_pInstance;
}

void LogSystem::AttachLogger( ILogger *pLogger ){
    try{
        std::lock_guard<decltype(m_LoggerLocker)> al( m_LoggerLocker );
        
        m_Loggers.push_back( pLogger );        
    }catch( ... ){        
    }
}

void LogSystem::DetachLogger( ILogger *pLogger ){
    try{
        std::lock_guard<decltype(m_LoggerLocker)> al( m_LoggerLocker );
        for( auto itr = m_Loggers.begin(); itr != m_Loggers.end(); itr++ ){
            if( *itr == pLogger ){
                m_Loggers.erase( itr );
                break;
            }
        }   
    }catch(...){        
    }
}

//void LogSystem::WriteLog( ILogger::LogLevel logLevel, const char* pFmt, ... ){
//    va_list ap;
//    va_start(ap,pFmt);
//    
//    WriteLogImpl( logLevel, pFmt, ap );
//    
//    va_end(ap);
//}
//
//void LogSystem::WriteLogImpl( ILogger::LogLevel logLevel, const char* pFmt, va_list& ap ){
//    
//    if( logLevel < m_LogLevel)
//        return;
//    try{
//        std::shared_ptr<LogAgentTask> task = std::make_shared<LogAgentTask>();
//        task->m_Level = logLevel;         
//        task->m_ThreadID = CurrentThread::GetThreadID();
//        
//        StringUtil::StringFormatEx( task->m_Msg, pFmt, ap );
//        
//        m_Queue.Push( task );        
//    }catch( ... ){        
//    }
//}

void LogSystem::WriteLogString( const char* pMsg ){    
    
    try{
        std::unique_lock<decltype(m_LoggerLocker)> al( m_LoggerLocker );
        
        for( auto itr = m_Loggers.begin(); itr != m_Loggers.end(); ++itr){
            (*itr)->WriteLogString( pMsg );
        }        
        
    }catch( ... ){        
    }
}

void LogSystem::Abort(){
    
    std::lock_guard<decltype(m_AbortLocker)> al( m_AbortLocker );
    
    m_Queue.Abort();
    
    m_Queue.Flush( std::bind( &LogSystem::SendToLogger, this, std::placeholders::_1) );
}

void LogSystem::SendToLogger(const std::shared_ptr<LogAgentTask>& task ){
    std::string lm;
    lm.reserve(300);
    std::unique_lock<decltype(m_LoggerLocker)> al( m_LoggerLocker );        
    for( auto itr = m_Loggers.begin(); itr != m_Loggers.end(); ++itr){
        StringTemplateUtil::Format( lm, "%04d-%02d-%02d-%02d:%02d:%02d.%03d.%03d [%06s] [%06d] [%s] %s\r\n",
                task->m_Now.GetYear(), task->m_Now.GetMonth(), task->m_Now.GetDay(), 
                task->m_Now.GetHour(), task->m_Now.GetMinute(), task->m_Now.GetSecond(), task->m_Now.GetMillisecond(), task->m_Now.GetMicrosecond(),
                ILogger::g_LogLevel[task->m_Level], task->m_ThreadID, (*itr)->GetName().c_str(), 
                task->m_Msg.c_str() );
        (*itr)->WriteLogString( lm.c_str() );
    } 
}

void LogSystem::WriteLogThread(){    
    while(true){
        try{
            auto task = m_Queue.Pop();
            if( task != NULL ){
                SendToLogger( task );
            }
        }catch(...){
            break;
        }
    }
}


// LogSystemEx
std::mutex LogSystemEx::m_LockInstance;
std::string LogSystemEx::m_DefaultKey;
std::map<std::string, std::shared_ptr<LogSystemEx>> LogSystemEx::m_InstancePool;

std::shared_ptr<LogSystemEx> LogSystemEx::GetInstance( const std::string& key ){
    std::lock_guard<decltype(m_LockInstance)> al( m_LockInstance );
    
    const std::string& usingKey = key.empty() ? m_DefaultKey : key;
    
    auto itrInstance = m_InstancePool.find( usingKey );
    if( itrInstance == m_InstancePool.end() ){
        // 因為 constructor 是 protected，所以無法用 std::make_shared
        std::shared_ptr<LogSystemEx> newInstance( new LogSystemEx() );
        m_InstancePool.insert( std::make_pair(usingKey,newInstance) );
        return newInstance;
    }else
        return itrInstance->second;
}

LogSystemEx::LogSystemEx():m_LogLevel(ILogger::Level_Debug){
    std::thread tLogThread( std::bind(&LogSystemEx::WriteLogThread,this) );
    tLogThread.detach();
}

LogSystemEx::~LogSystemEx(){
    m_Queue.Abort();
}

int LogSystemEx::Count() const{
    
    int count = 0;
    std::lock_guard<decltype(m_LoggerLocker)> al( m_LoggerLocker );    
    count = m_Loggers.size();    
    return count;
}

void LogSystemEx::AttachLogger( const GeneralLogger& logger ){
    try{
        std::lock_guard<decltype(m_LoggerLocker)> al( m_LoggerLocker );
        
        m_Loggers.push_back( logger );        
    }catch( ... ){        
    }
}

void LogSystemEx::DetachLogger( const GeneralLogger& logger ){
    try{
        std::lock_guard<decltype(m_LoggerLocker)> al( m_LoggerLocker );
                
        for( auto itr = m_Loggers.begin(); itr != m_Loggers.end(); itr++ ){
            if( itr->IsSameInstance( logger ) ){
                m_Loggers.erase(itr);
                break;
            }
        }
    }catch(...){        
    }
}

void LogSystemEx::WriteLog( ILogger::LogLevel logLevel, const char* pFmt, ... ){
    va_list ap;
    va_start(ap,pFmt);
    
    WriteLog( logLevel, pFmt, ap );
    
    va_end(ap);
}

void LogSystemEx::WriteLog( ILogger::LogLevel logLevel, const char* pFmt, va_list& ap ){
    if( logLevel < m_LogLevel)
        return;
    try{
        std::shared_ptr<LogAgentTask> task = std::make_shared<LogAgentTask>();
        task->m_Level = logLevel;    
        //task->m_Msg = pResult;          
        StringUtil::StringFormatEx( task->m_Msg, pFmt, ap, 300 );
        task->m_ThreadID = CurrentThread::GetThreadID();

        m_Queue.Push( task );
    }catch( ... ){        
    }    
}

void LogSystemEx::WriteLogThread(){
    std::string lm;
    lm.reserve(300);
    while(true){
        try{
            auto task = m_Queue.Pop();
            if( task != NULL ){
                std::lock_guard<decltype(m_LoggerLocker)> al( m_LoggerLocker );        
                for( auto itr = m_Loggers.begin(); itr != m_Loggers.end(); ++itr){
                    StringTemplateUtil::Format( lm, "%04d-%02d-%02d-%02d:%02d:%02d.%03d.%03d [%06s] [%06d] [%s] %s\r\n",
                            task->m_Now.GetYear(), task->m_Now.GetMonth(), task->m_Now.GetDay(), 
                            task->m_Now.GetHour(), task->m_Now.GetMinute(), task->m_Now.GetSecond(), task->m_Now.GetMillisecond(), task->m_Now.GetMicrosecond(),
                            ILogger::g_LogLevel[task->m_Level], task->m_ThreadID, itr->GetLoggerName().c_str(), 
                            task->m_Msg.c_str() );
                    itr->Write( lm.c_str(), lm.length() );
                } 
            }
        }catch(...){
            break;
        }
    }
}

}

