/* 
 * File:   IniFile.h
 * Author: 0007989
 *
 * Created on 2014年4月18日, 下午 5:12
 */

#ifndef UTILITY_INIFILE_H
#define	UTILITY_INIFILE_H

#include <list>
#include <map>
#include <string>
#include "IniSection.h"

namespace KGI_TW_Der_Utility
{   

class IniFile {
public:
    IniFile();
    IniFile(const IniFile& orig);
    virtual ~IniFile();
    
    const IniSection& operator[]( const std::string& name ) const;
    const IniSection& operator[]( const char *pName ) const;
    
    void Clear();
    void AddSection( IniSection& element );
    const std::list<IniSection *>& GetSections() const;
    const IniSection& GetSection(const std::string& name ) const;
    bool TryGetSection( const std::string& name, IniSection& result ) const;
private:
    std::list<IniSection *> m_Sections;
    std::map<std::string,IniSection *> m_SectionIndex;
};

}
#endif	/* UTILITY_INIFILE_H */

