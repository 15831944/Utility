/* 
 * File:   IniElement.h
 * Author: 0007989
 *
 * Created on 2014年4月18日, 下午 5:14
 */

#ifndef UTILITY_INIELEMENT_H
#define	UTILITY_INIELEMENT_H

#include <list>
#include <string>

namespace KGI_TW_Der_Utility
{   

class IniElement {
public:
    enum class ElementTypeEnum {        
        KeyValue,
        Comment,
        Other
    };
public:    
    IniElement(const IniElement& orig);
    virtual ~IniElement();
    
    IniElement& operator=( const IniElement& orig );
    
    virtual IniElement *Duplicate() const = 0;    
    virtual std::string ToString() const = 0;
    
    ElementTypeEnum GetElementType() const;    
protected:
    IniElement(); // can not be instanciated
protected:    
    ElementTypeEnum m_ElementType;

};

}
#endif	/* UTILITY_INIELEMENT_H */

