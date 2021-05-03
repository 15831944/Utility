/* 
 * File:   IniKeyValue.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月18日, 下午 5:13
 */

#include "IniKeyValue.h"

#include "../Log/LogMessage.h"

namespace KGI_TW_Der_Utility
{   

IniKeyValue::IniKeyValue() {
    m_ElementType = ElementTypeEnum::KeyValue;
}

IniKeyValue::IniKeyValue( const char *pKey, const char *pValue ){
    m_ElementType = ElementTypeEnum::KeyValue;
    
    m_Key = pKey;
    m_Value = pValue;
}

IniKeyValue::IniKeyValue(const IniKeyValue& orig) {
    *this = orig;
}

IniKeyValue::~IniKeyValue() {
}

IniElement *IniKeyValue::Duplicate() const {
    IniKeyValue *pKeyValue = new IniKeyValue( *this );
    return pKeyValue;
}

IniKeyValue& IniKeyValue::operator=( const IniKeyValue& orig ){
    if( this != &orig )
    {
        m_Key = orig.m_Key;
        m_Value = orig.m_Value;
        
        IniElement& lhs = *this;
        const IniElement& rhs = orig;
        lhs = rhs; // invoke IniElement::operator=()
    }
    return *this;
}

std::string IniKeyValue::ToString() const {
    LogMessage lm;
    lm.Format( "%s=%s", m_Key.c_str(), m_Value.c_str() );
    std::string tmp = lm.GetMessage();
    return tmp;
}

}