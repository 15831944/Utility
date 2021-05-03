/* 
 * File:   IniKeyValue.h
 * Author: 0007989
 *
 * Created on 2014年4月18日, 下午 5:13
 */

#ifndef UTILITY_INIKEYVALUE_H
#define	UTILITY_INIKEYVALUE_H

#include "IniElement.h"
#include "../MacroRecipe.h"

namespace KGI_TW_Der_Utility
{   

class IniKeyValue : public IniElement {
public:
    IniKeyValue();
    IniKeyValue( const char *pKey, const char *pValue );
    IniKeyValue(const IniKeyValue& orig);
    virtual ~IniKeyValue();
    
    IniKeyValue& operator=( const IniKeyValue& orig );
    virtual IniElement *Duplicate() const;
    
    virtual std::string ToString() const;
    
    PROP_GETSET_INOUT( std::string&, std::string&, Key, m_Key );
    PROP_GETSET_INOUT( std::string&, std::string&, Value, m_Value );
private:
    std::string m_Key;
    std::string m_Value;

};

}
#endif	/* UTILITY_INIKEYVALUE_H */

