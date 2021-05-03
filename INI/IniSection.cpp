/* 
 * File:   IniSection.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月18日, 下午 5:12
 */

#include "IniSection.h"

#include "../Log/LogMessage.h"
#include "IniKeyValue.h"

namespace KGI_TW_Der_Utility
{   
    
std::string IniSection::m_EmptyValue;

IniSection::IniSection() {    
}

IniSection::IniSection( const char *pName ){    
    m_Name = pName;
}

IniSection::IniSection(const IniSection& orig) {
    *this = orig;
}

IniSection::~IniSection() {
    for( std::list< IniElement* >::iterator itr = m_SubElements.begin(); itr != m_SubElements.end(); ++itr )
        delete *itr;
}

IniSection& IniSection::operator=( const IniSection& orig ){
    if( this != &orig )
    {
        m_Name = orig.m_Name;
        
        m_SubElements.clear();
        for( std::list<IniElement*>::const_iterator itr = orig.m_SubElements.begin(); itr != orig.m_SubElements.end(); ++itr )
        {
            m_SubElements.push_back( (*itr)->Duplicate() );
        }
    }
    return *this;
}

std::string IniSection::ToString() const{
    LogMessage lm;
    lm.Format( "[%s]", m_Name.c_str() );
    std::string tmp = lm.GetMessage();
    return tmp;
}

void IniSection::AddElement( IniElement& element ){    
    m_SubElements.push_back( element.Duplicate() );
}

const std::list<IniElement*>& IniSection::GetElements() const{
    return m_SubElements;
}

const std::string& IniSection::operator[](const char *pKey) const{
    for( std::list<IniElement*>::const_iterator itr = m_SubElements.begin(); itr != m_SubElements.end(); ++itr )
    {
        if( (*itr)->GetElementType() == IniElement::ElementTypeEnum::KeyValue )
        {
            IniKeyValue *pKV = (IniKeyValue*)(*itr);
            if( pKV->GetKey() == pKey )
                return pKV->GetValue();
        }
    }
    return m_EmptyValue;
}

const std::string& IniSection::operator[](std::string& key) const{
    return (*this)[key.c_str()];
}

bool IniSection::TryGetElement( const std::string& name, std::string& value ) const{
    for( std::list<IniElement*>::const_iterator itr = m_SubElements.begin(); itr != m_SubElements.end(); ++itr )
    {
        if( (*itr)->GetElementType() == IniElement::ElementTypeEnum::KeyValue )
        {
            IniKeyValue *pKV = (IniKeyValue*)(*itr);
            if( pKV->GetKey() == name )
            {
                value = pKV->GetValue();
                return true;
            }
        }
    }
    return false;
}

}