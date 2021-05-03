/* 
 * File:   IniSection.h
 * Author: 0007989
 *
 * Created on 2014年4月18日, 下午 5:12
 */

#ifndef UTILITY_INISECTION_H
#define	UTILITY_INISECTION_H

#include "IniElement.h"
#include "IniKeyValue.h"
#include "../MacroRecipe.h"

namespace KGI_TW_Der_Utility
{   

class IniSection {
public:
    IniSection();
    IniSection( const char *pName );
    IniSection(const IniSection& orig);
    virtual ~IniSection();
    
    IniSection& operator=( const IniSection& orig );    
    
    virtual std::string ToString() const;
    
    const std::string& operator[](const char *pKey) const;
    const std::string& operator[](std::string& key) const;
    
    PROP_GETSET( std::string, Name , m_Name );
    void AddElement( IniElement& element );
    const std::list<IniElement*>& GetElements() const;
    
    bool TryGetElement( const std::string& name, std::string& value ) const;
private:
    std::string m_Name;
    std::list<IniElement *> m_SubElements;
    static std::string m_EmptyValue;

};

}
#endif	/* UTILITY_INISECTION_H */

