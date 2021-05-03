/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DeferedLogger.h
 * Author: Justin Lin
 *
 * Created on 2017年6月2日, 上午 10:50
 */

#ifndef UTILITY_DEFEREDLOGGER_H
#define UTILITY_DEFEREDLOGGER_H

#include "ILogger.h"
#include "../Collect/BlockingQueue.h"

namespace KGI_TW_Der_Utility{

/// \brief 延後產生要寫入Log內的Log訊息的類別
/// 配合 ILogData 來使用，使用端可以自行繼承自 ILogData 並產生相關要寫入 Log 的資料物件，並透過 DeferedLogger::WriteLog 來寫入
///
class DeferedLogger : public ILogger {
public:
    DeferedLogger();
    DeferedLogger(const DeferedLogger& orig) = delete;
    virtual ~DeferedLogger();
    
    // override from ILogger
    virtual void WriteLog( LogLevel logLevel, const std::string& msg );
    virtual void WriteLog( LogLevel logLevel, const char *pFmt, ... );
    virtual void WriteLogImpl( LogLevel logLevel, const char *pFmt, va_list &arg );
    virtual void WriteLog( LogLevel logLevel, std::shared_ptr<ILogData> logData );
    // end
private:
    /// \brief 另一個用來將 ILogData 物件產生要寫入 Log 裏的訊息並寫出到 ILogger 物件中的執行緒
    void DeferedWriteLogThread();
    
private:
    BlockingQueue<std::shared_ptr<ILogData>>    LogDataQueue_;      //!< 暫存要寫入 Log 中的 ILogData 的queue

};

}
#endif /* UTILITY_DEFEREDLOGGER_H */

