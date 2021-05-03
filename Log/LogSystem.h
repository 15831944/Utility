/* 
 * File:   LogSystem.h
 * Author: 0007989
 *
 * Created on 2014年1月27日, 下午 2:51
 */

#ifndef UTILITY_LOGSYSTEM_H
#define	UTILITY_LOGSYSTEM_H

#include "ILogger.h"
#include <mutex>
#include <map>
#include <vector>
#include <thread>
#include <stdarg.h>
#include "../MacroRecipe.h"
#include "../DateTime.h"
#include "../Collect/BlockingQueue.h"
#include "GeneralLogger.h"
#include "../StringTemplateUtil.h"
#include "../MT/CurrentThread.h"

namespace KGI_TW_Der_Utility{      

/// LogSystem 
/// Singleton instance of LogSystem to be used in every process.
///
/// Use LogSystem::GetInstance() to get the instance of LogSystem and using it for logging information.
class LogSystem{
    
    /// \brief 配合 LogAgent 使用用來記錄要寫入 ILogger 中的Log資訊
    class LogAgentTask{
    public:    
        DateTime m_Now;                     //!< Log時間
        std::string m_Msg;                  //!< 要寫入的Log訊息
        ILogger::LogLevel m_Level;          //!< Log層級
		THREAD_ID_TYPE m_ThreadID;                   //!< the thread id for send this message
    };

    friend class LoggerWrapper;
public:        
    /// Destructor
    virtual ~LogSystem();
    
    /// Attache logger to LogSystem
    /// \param pLogger the ILogger instance to be attached into LogSystem
    void AttachLogger( ILogger *pLogger );
    /// Detach logger from LogSystem
    /// \param pLogger the ILogger instance that will be detached from LogSystem
    void DetachLogger( ILogger *pLogger );
    /// Get the total number of ILogger that is attached in the LogSystem
    /// \return the total number of ILooger in LogSystem
    int Count() const;    
    
//    void WriteLog( ILogger::LogLevel logLevel, const char* pFmt, ... );
//    void WriteLogImpl( ILogger::LogLevel logLevel, const char* pFmt, va_list& ap ); 
    
    template< typename ... Args >
    void WriteLogEx( ILogger::LogLevel logLevel, const char* pFmt, Args const & ... args ){
        if( logLevel < m_LogLevel ) return;
        
        std::shared_ptr<LogAgentTask> task = std::make_shared<LogAgentTask>();
        task->m_Level = logLevel;    
        StringTemplateUtil::Format( task->m_Msg, pFmt, args ... );                
        task->m_ThreadID = CurrentThread::GetThreadID();
        
        m_Queue.Push( task );
    }

    /// Singleton的物件沒有機會執行到destructor，所以要呼叫Terminate()讓LogSystem物件有機會呼叫到destructor
    static void Terminate();
    /// Get the singleton instance of LogSystem
    /// \return LogSystem instance
    static LogSystem &GetInstance();
    
    PROP_GETSET_BYVALUE( ILogger::LogLevel, LogLevel, m_LogLevel );
    
    void Abort();
protected:
    /// private constructor
    LogSystem();  
    void WriteLogString( const char* pMsg );
    
    /// \brief 負責寫入Log中的執行緒
    void WriteLogThread();
    void SendToLogger(const std::shared_ptr<LogAgentTask>& task );
    
private:
    static LogSystem *m_pInstance;              //!< the singleton instance
    static std::mutex m_InstanceLocker;             //!< lock for synchronize logger singletone

    static int m_LoggerID;                      //!< the last ID for logger
    
    std::vector<ILogger*> m_Loggers;            //!< all attached ILogger
    mutable std::mutex m_LoggerLocker;          //!< lock object for synchronize logger vector    
    
    ILogger::LogLevel m_LogLevel;               //!< current log level
        
    std::thread m_WriteLogThread;
    BlockingQueue<std::shared_ptr<LogAgentTask>> m_Queue;       //!< 儲存寫入Log的資料的Queue
    mutable std::mutex m_AbortLocker;
};



/// \brief 配合GeneralLogger，以及AbstractFilePersistence使用的Log管理
/// 可以支援多組的LogSystem
class LogSystemEx{        
public:
    ~LogSystemEx();
    
    /// Attache logger to LogSystem
    /// \param pLogger the ILogger instance to be attached into LogSystem
    void AttachLogger( const GeneralLogger& logger );
    /// Detach logger from LogSystem
    /// \param pLogger the ILogger instance that will be detached from LogSystem
    void DetachLogger( const GeneralLogger& logger );
    /// Get the total number of ILogger that is attached in the LogSystem
    /// \return the total number of ILooger in LogSystem
    int Count() const;
    
    static std::shared_ptr<LogSystemEx> GetInstance( const std::string& key = "" );
    
    ILogger::LogLevel GetLogLevel() const { return m_LogLevel; }
    void SetLogLevel( ILogger::LogLevel logLevel ){ m_LogLevel = logLevel; }
    
    void WriteLog( ILogger::LogLevel logLevel, const char* pFmt, ... );
    void WriteLog( ILogger::LogLevel logLevel, const char* pFmt, va_list& ap );   
    
protected:
    LogSystemEx();    
    
    /// \brief 負責寫入Log中的執行緒
    void WriteLogThread();
    
protected:
    /// \brief 配合 LogAgent 使用用來記錄要寫入 ILogger 中的Log資訊
    class LogAgentTask{
    public:    
        DateTime m_Now;                     //!< Log時間
        std::string m_Msg;                  //!< 要寫入的Log訊息
        ILogger::LogLevel m_Level;          //!< Log層級
	THREAD_ID_TYPE m_ThreadID;                   //!< the thread if for send this message
    };
    static std::mutex m_LockInstance;  
    static std::string m_DefaultKey;
    static std::map<std::string, std::shared_ptr<LogSystemEx>> m_InstancePool;
    
    mutable std::mutex m_LoggerLocker;
    std::vector<GeneralLogger> m_Loggers;            //!< all attached ILogger
    ILogger::LogLevel m_LogLevel;
    BlockingQueue<std::shared_ptr<LogAgentTask>> m_Queue;       //!< 儲存寫入Log的資料的Queue
};


}

#endif	/* UTILITY_LOGSYSTEM_H */

