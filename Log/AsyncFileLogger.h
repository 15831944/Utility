/* 
 * File:   AsyncFileLogger.h
 * Author: 0007989
 *
 * Created on 2014年8月18日, 上午 8:31
 */

#ifndef UTILITY_ASYNCFILELOGGER_H
#define	UTILITY_ASYNCFILELOGGER_H

#include <string>
#include "AsyncRollFileLogger.h"
#include "FileLogger.h"
#include "../Collect/BlockingQueue.h"
#include <memory>
#include <thread>

namespace ProgLab{ namespace Utility{   

class AsyncFileLogger : public AsyncRollFileLogger {    
public:
    AsyncFileLogger(const char *pFilenamePattern, const char *pLoggerName = NULL );
    virtual ~AsyncFileLogger();
    
};
    
//class AsyncFileLogger : public FileLogger{    
//public:
//    AsyncFileLogger(const std::string& filename );    
//    AsyncFileLogger(const std::string& filename, const std::string& loggerName );    
//    virtual ~AsyncFileLogger();
//    
//    virtual bool Open();
//    virtual void Close();
//    
//    virtual void WriteLogString( const char* pMsg );
//protected:
//    void WriteLogThread();
//protected:
//    BlockingQueue<std::shared_ptr<char>> m_Queue;
//    std::shared_ptr<int> m_ShareLock;
//    
//};

}
#endif	/* UTILITY_ASYNCFILELOGGER_H */

