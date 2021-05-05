/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ExecuteResult.h
 * Author: Justin Lin
 *
 * Created on 2018年6月20日, 下午 9:56
 */

#ifndef UTILITY_EXECUTERESULT_H
#define UTILITY_EXECUTERESULT_H

#include <string>

#include "StringTemplateUtil.h"

namespace ProgLab{ namespace Utility{     

class ExecuteResult {
public:
    ExecuteResult( bool result, const std::string& resultMsg );
    
    template< typename ... Args >
    explicit ExecuteResult(bool result,const char* pFmt, Args const & ... args ):Result_(result){
        StringTemplateUtil::Format( ResultMessage_, pFmt, args... );
    }
    
    ExecuteResult(const ExecuteResult& orig) = default;
    virtual ~ExecuteResult();
    
    operator bool() const{ return Result_; }
    void SetResult( bool result ){ Result_ = result; }
    void SetResultMessage( const std::string&& resultMessage ){ ResultMessage_ = std::move(resultMessage); }
    const std::string& GetResultMessage() const{ return ResultMessage_; }
    
    template< typename ... Args >
    void FormatMessage(const char* pFmt, Args const & ... args ){
        StringTemplateUtil::Format( ResultMessage_, pFmt, args... );
    }
private:
    bool            Result_;
    std::string     ResultMessage_;
};

}
#endif /* UTILITY_EXECUTERESULT_H */

