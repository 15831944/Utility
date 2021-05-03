/* 
 * File:   CommonException.h
 * Author: 0007989
 *
 * Created on 2014年3月13日, 上午 10:11
 */

#ifndef UTILITY_COMMONEXCEPTION_H
#define	UTILITY_COMMONEXCEPTION_H

#include <string>
#include <stdexcept>

namespace KGI_TW_Der_Utility
{   

class CommonException : public std::exception {
public:
    CommonException() throw() : std::exception(){}
    CommonException( int lineNum, const char *pSource, const char *pMsg );
    CommonException(const CommonException& orig);
    virtual ~CommonException() throw();
    
    CommonException& operator=( const CommonException& orig );
    
    int GetLineNum() const;
    std::string GetSource() const;
    std::string GetMessage() const;
    virtual const char* what() const throw() { return m_Message.c_str(); }
private:
    int m_LineNum;
    std::string m_Source;
    std::string m_Message;    
};

}

#endif	/* UTILITY_COMMONEXCEPTION_H */

