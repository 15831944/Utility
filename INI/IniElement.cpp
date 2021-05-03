/* 
 * File:   IniElement.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月18日, 下午 5:14
 */

#include "IniElement.h"

namespace KGI_TW_Der_Utility
{   

IniElement::IniElement() {
    m_ElementType = ElementTypeEnum::Other;
}

IniElement::IniElement(const IniElement& orig) {
    *this = orig;
}

IniElement::~IniElement() {
}

IniElement& IniElement::operator=( const IniElement& orig ){
    if( this != &orig )
    {
        m_ElementType = orig.m_ElementType;     
    }
    return *this;
}

IniElement::ElementTypeEnum IniElement::GetElementType() const{
    return m_ElementType;
    
}

IniElement *IniElement::Duplicate() const{
    return NULL;
}

}