/* 
 * File:   LogAgent.h
 * Author: 0007989
 *
 * Created on 2015年1月15日, 下午 4:56
 */

#ifndef UTILITY_LOGAGENT_H
#define	UTILITY_LOGAGENT_H

#include "../Collect/BlockingQueue.h"
#include "LoggerWrapper.h"
#include "../DateTime.h"
#include "../MT/CurrentThread.h"
#include "../MT/ManualResetEvent.h"

namespace KGI_TW_Der_Utility {

class ILogger;

/// \brief 配合 LogAgent 使用用來記錄要寫入 ILogger 中的Log資訊
class LogAgentTask{
public:    
    DateTime m_Now;                     //!< Log時間
    std::string m_Msg;                  //!< 要寫入的Log訊息
    ILogger::LogLevel m_Level;          //!< Log層級
    THREAD_ID_TYPE m_ThreadID;          //!< 寫入Log的執行緒ID
};

/// \brief 使用另一個執行緒來寫入Log
class LogAgent{
public:
    LogAgent():m_pLogger(NULL){}
    ~LogAgent(){}
    
    /// \brief 設定要使用的ILogger物件，未指定時，就使用LogSystem::GetInstance())
    void SetLogger( ILogger* pLogger ){ m_pLogger = pLogger; }
    /// \brief 啟動一個執行緒來負責寫入Log
    void Start();
    /// \brief 停止執行緒
    void Stop();
   
    void Write( ILogger::LogLevel level, const char* pFmt, ... );
    void Write( ILogger::LogLevel level, const char* pFmt, va_list& ap );
    void WriteMessage( ILogger::LogLevel level, const char* pMsg );
    
private:
    /// \brief 負責寫入Log中的執行緒
    void WriteLogThread();
    void WriteOut( const std::shared_ptr<LogAgentTask>& task );
    
private:
    ILogger* m_pLogger;                                         //!< 目前要使用的ILogger物件
    BlockingQueue<std::shared_ptr<LogAgentTask>> m_Queue;       //!< 儲存寫入Log的資料的Queue
    LoggerWrapper m_DefaultLogger;                              //!< 未指定ILogger時使用的預設Logger
    ManualResetEvent m_EventWriteThreadAborted;
};

}
#endif	/* UTILITY_LOGAGENT_H */

