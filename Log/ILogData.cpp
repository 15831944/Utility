/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ILogData.cpp
 * Author: Justin Lin
 * 
 * Created on 2017年6月2日, 上午 9:33
 */

#include "ILogData.h"
#include "../DateTime.h"
#include "../StringTemplateUtil.h"
#include "../MT/CurrentThread.h"

namespace KGI_TW_Der_Utility{

ILogData::ILogData() {
    ThreadID_ = CurrentThread::GetThreadID();
}

ILogData::ILogData(const ILogData& orig) {
    *this = orig;
}

ILogData::~ILogData() {
}

ILogData& ILogData::operator=( const ILogData& rhs ){
    if( this != &rhs ){
        ThreadID_ = rhs.ThreadID_;
        LogLevelStr_ = rhs.LogLevelStr_;
        DateTimeStr_ = rhs.DateTimeStr_;
    }
    return *this;
}

std::string ILogData::MakeLogString( const std::string& logLevelStr, const std::string& dateTimeStr, const std::string& msg, const std::string& name, int threadID ) const {
    std::string result;
    StringTemplateUtil::Format( result, "%s [%s] [%06d] [%s] %s\r\n", dateTimeStr.c_str(), logLevelStr.c_str(), threadID, name.c_str(), msg.c_str() );
    return result;
}

StringLogData::StringLogData( const std::string& msg ):LogMsg_(msg){    
}

StringLogData::StringLogData( const StringLogData& orig ):ILogData(orig){
    *this = orig;
}
    
StringLogData::~StringLogData(){
}

StringLogData& StringLogData::operator=( const StringLogData& rhs ){
    ILogData::operator =( rhs );
    if( this != &rhs ){
        LogMsg_ = rhs.LogMsg_;
    }
    return *this;
}

std::string StringLogData::ToString() const{
    return MakeLogString( LogLevelStr_, DateTimeStr_, LogMsg_, "", ThreadID_ );
}

}
