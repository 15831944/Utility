/* 
 * File:   IniComment.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月23日, 下午 5:08
 */

#include "IniComment.h"

namespace KGI_TW_Der_Utility
{   

IniComment::IniComment() {
    m_ElementType = ElementTypeEnum::Comment;
}

IniComment::IniComment( const char *pComment ){
    m_ElementType = ElementTypeEnum::Comment;
    m_Comment = pComment;
}

IniComment::IniComment( const std::string& comment ){
    m_ElementType = ElementTypeEnum::Comment;
    m_Comment = comment;
}

IniComment::IniComment(const IniComment& orig) {
    *this = orig;
}

IniComment::~IniComment() {
}

IniComment& IniComment::operator=( const IniComment& orig ){
    if( this != &orig )
    {
        m_Comment = orig.m_Comment;
        
        ((IniElement&)(*this)) = (const IniElement&)orig;
        
    }
    return *this;
}

IniElement *IniComment::Duplicate() const{
    IniComment *pNew = new IniComment( *this );
    return pNew;
}
    
std::string IniComment::ToString() const{
    std::string result = "#" + m_Comment;
    return result;
}

}