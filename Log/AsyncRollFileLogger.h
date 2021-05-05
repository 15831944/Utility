/* 
 * File:   AsyncRollFileLogger.h
 * Author: 0007989
 *
 * Created on 2014年8月18日, 上午 10:29
 */

#ifndef UTILITY_ASYNCROLLFILELOGGER_H
#define	UTILITY_ASYNCROLLFILELOGGER_H

#define _ASYNCROLLFILELOGGER_H_Use_Buffer_                // use swap buffer for write log

#include "ILogger.h"
#include "DeferedLogger.h"

#include <string>
#include <cstdio>
#include <memory>

#include "FileLogger.h"
#include "FileBufferLogger.h"
#include <mutex>
#include "../MT/DataDispatcher.h"
#include "../Collect/BlockingQueue.h"
#include "../Persistence/RollFilePersistence.h"
#include <memory>
#include <thread>

namespace ProgLab{ namespace Utility{   

typedef FileLogger _BaseFileLogType_;

class AsyncRollFileLogger : public _BaseFileLogType_
{
public:
    /// \brief if maxSizeMB is zero, then it works as AsyncFileLogger
    AsyncRollFileLogger( const char *pFilenamePattern, int maxSizeMB = 0, const char *pLoggerName = NULL );    
    virtual ~AsyncRollFileLogger();    
    
    virtual bool Open();
    virtual void Close();

    virtual void WriteLogString( const char* pMsg );    
private:    
    std::string m_FilenamePattern;
    int m_MaxSizeMB;
    int m_LastIndex;        //!< the last index of rolled log file
    mutable std::mutex m_FileOperation;  
    int m_IsThreadReady;
#ifdef _ASYNCROLLFILELOGGER_H_Use_Buffer_
    struct WriteBufferData{
        char* pBuffer;
        int Size;
    };
    BlockingQueue<WriteBufferData> m_Queue;
#else
    BlockingQueue<std::shared_ptr<char>> m_Queue;
#endif
    
    static const int m_BufferSize = 1024 * 1024;
    char* m_pBuffer1;
    char* m_pBuffer2;
    char* m_pCrtBuffer;
    int m_CrtBufferPosition;
    int m_WriteBufferSize;
    mutable std::mutex m_BufferOperation;
    ManualResetEvent m_DumpBuffer;
private:
    void WriteLogThread();   
#ifdef _ASYNCROLLFILELOGGER_H_Use_Buffer_
    void TimerWriteLogThread();
    void SwitchBuffer();
#endif
    
};

class AsyncRollFileLoggerEx : public ILogger{
public:
    AsyncRollFileLoggerEx( const char *pFilenamePattern, int maxSizeMB = 0, const char *pLoggerName = NULL );    
    virtual ~AsyncRollFileLoggerEx();    
    
    virtual bool Open();
    virtual void Close();

    virtual void WriteLogString( const char* pMsg );    
private:
    mutable std::mutex m_LockPersistence;
    AsyncRollFilePersistence* m_pPersistence;
    std::string m_FilenamePattern;
};

/// \brief 繼承自 DeferedLogger 的非同寫入及可Roll的Logger
/// 內部是使用 AsyncRollFilePersistence 來儲存 Log
class DeferedAsyncRollFileLoggerEx : public DeferedLogger{
public:
    DeferedAsyncRollFileLoggerEx( const char *pFilenamePattern, int maxSizeMB = 0, const char *pLoggerName = NULL );    
    virtual ~DeferedAsyncRollFileLoggerEx();    
    
    virtual bool Open();
    virtual void Close();

    virtual void WriteLogString( const char* pMsg );    
private:
    mutable std::mutex          m_LockPersistence;      //!< 用來管控 m_pPersistence 的 lock
    AsyncRollFilePersistence*   m_pPersistence;         //!< 目前用來寫入資料的 AbstractFilePersistence
    std::string                 m_FilenamePattern;      //!< Log 檔案名稱的樣式
};
}

#endif	/* UTILITY_ASYNCROLLFILELOGGER_H */

