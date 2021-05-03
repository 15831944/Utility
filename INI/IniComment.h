/* 
 * File:   IniComment.h
 * Author: 0007989
 *
 * Created on 2014年4月23日, 下午 5:08
 */

#ifndef UTILITY_INICOMMENT_H
#define	UTILITY_INICOMMENT_H

#include "IniElement.h"
#include "../MacroRecipe.h"

namespace KGI_TW_Der_Utility
{   

class IniComment : public IniElement {
public:
    IniComment();
    IniComment( const char *pComment );
    IniComment( const std::string& comment );
    IniComment(const IniComment& orig);    
    virtual ~IniComment();
    
    IniComment& operator=( const IniComment& orig );
    
    virtual IniElement *Duplicate() const;    
    virtual std::string ToString() const;
    
    PROP_GETSET_INOUT( std::string&, std::string&, Comment, m_Comment );
private:
    std::string m_Comment;

};

}
#endif	/* UTILITY_INICOMMENT_H */

