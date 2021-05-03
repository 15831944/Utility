/* 
 * File:   IniFile.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月18日, 下午 5:12
 */

#include "IniFile.h"

namespace KGI_TW_Der_Utility
{   

IniFile::IniFile() {
}

IniFile::IniFile(const IniFile& orig) {
}

IniFile::~IniFile() {
    Clear();
}

void IniFile::Clear(){
    
    for( std::list<IniSection*>::iterator itr = m_Sections.begin(); itr != m_Sections.end(); ++itr )
    {
        delete (*itr);
    }
    
    m_Sections.clear();
    
    m_SectionIndex.clear();
}

void IniFile::AddSection( IniSection& element ){
    IniSection* pSection = new IniSection(element);
    m_Sections.push_back( pSection );
    try    
    {
        //DEBUG
        std::string tmp = pSection->GetName();
        m_SectionIndex.insert( std::pair<std::string,IniSection*>( tmp, pSection ) );
    }catch(...)
    {
        throw;
    }
}
    
const std::list<IniSection *>& IniFile::GetSections() const{
    return m_Sections;
}

const IniSection& IniFile::GetSection(const std::string& name ) const{
    
    // In const function, only can use const iterator
    std::map<std::string,IniSection *>::const_iterator found = m_SectionIndex.find( name );
    if( found != m_SectionIndex.end() )
        return *(found->second);
    else
        throw "Can not find specified section";
}

const IniSection& IniFile::operator[]( const std::string& name ) const{
    return GetSection( name );
}
    
const IniSection& IniFile::operator[]( const char *pName ) const{
    std::string name = pName;
    return GetSection( name );
}

bool IniFile::TryGetSection( const std::string& name, IniSection& result ) const{
    std::map<std::string,IniSection *>::const_iterator found = m_SectionIndex.find( name );
    if( found == m_SectionIndex.end() )
        return false;
    else
    {
        result = *(found->second); // copy 
        return true;
    }
}

}