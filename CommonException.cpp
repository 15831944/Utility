/* 
 * File:   CommonException.cpp
 * Author: 0007989
 * 
 * Created on 2014年3月13日, 上午 10:11
 */

#include "CommonException.h"

namespace KGI_TW_Der_Utility
{   

CommonException::CommonException(int lineNum, const char *pSource, const char *pMsg ){
    m_LineNum = lineNum;
    m_Source.assign( pSource );
    m_Message.assign( pMsg );
}

CommonException::CommonException(const CommonException& orig)  : std::exception(orig){
    m_LineNum = orig.m_LineNum;
    m_Source = orig.m_Source;
    m_Message = orig.m_Message;
}

CommonException::~CommonException() throw() {
}

CommonException& CommonException::operator=( const CommonException& orig ){
    if( this != &orig )
    {
        m_LineNum = orig.m_LineNum;
        m_Source = orig.m_Source;
        m_Message = orig.m_Message;
    }
    return *this;
}

int CommonException::GetLineNum() const{
    return m_LineNum;
}

std::string CommonException::GetSource() const{
    return m_Source;
}

std::string CommonException::GetMessage() const{
    return m_Message;
}

}