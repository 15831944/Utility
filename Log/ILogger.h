/* 
 * File:   ILogger.h
 * Author: 0007989
 *
 * Created on 2014年1月27日, 下午 2:42
 */

#ifndef UTILITY_ILOGGER_H
#define	UTILITY_ILOGGER_H

#include <stdarg.h>
#include <string>
#include <memory>

#include "ILogData.h"
#include "../StringTemplateUtil.h"

namespace KGI_TW_Der_Utility
{   

    /// \brief Define the interface for ILogger
    ///
    /// ILogger is an interface for logging message
class ILogger {
public:
    /// Log message level enumeration
    enum LogLevel
    {
        Level_Debug,    //!< Debug level
        Level_Debug_3,  //!< Debug level 3
        Level_Debug_2,  //!< Debug level 2
        Level_Debug_1,  //!< Debug level 1
        Level_Warn,     //!< Warn level
        Level_Info,     //!< info level
        Level_Error,    //!< Error level
        Level_Fatal,    //!< Fatal level
                
    };
    
    static const char g_LogLevel[][10];    //!< log level string 
public:
    /// Constructor
    ILogger();    
    /// Constructor for specify the loggerName
    /// \param loggerName the name of logger and will be show in log
    ILogger(const std::string& loggerName);
    /// Destructor
    virtual ~ILogger();
    
    /// Get name of Logger
    /// \return the name of Logger
    const std::string& GetName() const;
    /// Set the name of Logger
    /// \param the name of Logger
    void SetName(const std::string &name);
    
    virtual LogLevel GetMaxLogLevel() const;
    void SetMaxLogLevel( LogLevel logLevel );
    
    /// Write log into ILogger
    /// \param logLevel
    /// \param msg message to be wrote into ILogger
    virtual void WriteLog( LogLevel logLevel, const std::string& msg );    
    /// \brief if the loglevel will not be logged, then the format of log string will not be processed. This will save some cpu time
    /// This function will internerally invoke the WriteLogImpl()
    virtual void WriteLog( LogLevel logLevel, const char *pFmt, ... );
    /// \brief will be invoked by other function with variable parameters, such as LogSystem::WriteLog( LogLevel,const char*, ... )
    virtual void WriteLogImpl( LogLevel logLevel, const char *pFmt, va_list &arg );
    /// \brief apply the log pattern
    virtual const char* MakeLogMessage( const char* pLevelString, const char* pMsg ) const;
    /// \brief 實際上寫入到 Log 物件中的函式，需由衍生類別進行實作
    virtual void WriteLogString( const char* pMsg ) = 0;    
    
    virtual void WriteLog( LogLevel logLevel, std::shared_ptr<ILogData> logData ){};
    
    template <typename ... Args>
    void OnLog(  LogLevel logLevel, const char* pFmt,Args const & ... args){
        std::string logMsg;
        StringTemplateUtil::Format( logMsg, pFmt, args ...);
        WriteLog( logLevel, logMsg );
    }
    
protected:
    std::string m_LoggerName;   //!< The name of Logger
    char *m_pDateStr;           //!< The current date string
    LogLevel m_MaxLogLevel;     //!< The maximum log level that this logger could accept    
protected:          
    /// \brief 格式化 Log 字串
    /// \param defaultSize 可以指定要預先將 result 參數保留多少字元組的空間，以避免格式化過長的字串時可能會因原本的長度不足而多執行一次格式化流程
    ///
    virtual void MakeLogMessage( std::string& result, const char* pLevelString, const char* pMsg, int defaultSize = 0 ) const;
private:
    void InitDateStr();
};

}

#endif	/* UTILITY_ILOGGER_H */

