/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ILogData.h
 * Author: Justin Lin
 *
 * Created on 2017年6月2日, 上午 9:33
 */

#ifndef UTILITY_ILOGDATA_H
#define UTILITY_ILOGDATA_H

#include <string>

namespace KGI_TW_Der_Utility{

class ILogData {
public:
    ILogData();
    ILogData(const ILogData& orig);
    virtual ~ILogData();
    
    ILogData& operator=( const ILogData& rhs );
    
    void SetDateTimeStr( const std::string& dateTimeStr ){
        DateTimeStr_ = dateTimeStr;
    }
    const std::string& GetDateTimeStr() const{ return DateTimeStr_; }
    void SetLogLevelStr( const std::string& logLevelStr ){
        LogLevelStr_ = logLevelStr;
    }
    const std::string& GetLogLevelStr() const{ return LogLevelStr_; }
    
    virtual std::string ToString() const = 0;
protected:    
    int                     ThreadID_;    
    std::string             LogLevelStr_;           //!< Log 層級字串 (由 ILogger 的實作來指定)
    std::string             DateTimeStr_;           //!< Log 發生時間點的字串表示內容 (由 ILogger 的實作來指定)
    
protected:
    std::string MakeLogString( const std::string& logLevelStr, const std::string& dateTimeStr, const std::string& msg, const std::string& name, int threadID = 0 ) const;

};

class StringLogData : public ILogData{
public:
    StringLogData( const std::string& msg );
    StringLogData( const StringLogData& orig );
    virtual ~StringLogData();
    
    StringLogData& operator=( const StringLogData& rhs );
    
    // override from ILogData
    virtual std::string ToString() const;
    // end
private:
    std::string         LogMsg_;
};

}

#endif /* UTILITY_ILOGDATA_H */

