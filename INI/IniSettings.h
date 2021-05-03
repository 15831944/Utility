/* 
 * File:   IniSettings.h
 * Author: 0007989
 *
 * Created on 2014年3月7日, 下午 7:18
 */

#ifndef UTILITY_INISETTINGS_H
#define	UTILITY_INISETTINGS_H

#include <string>
#include <cstdio>
//#include <regex.h>
#include "../RegexUtil.h"

#include "IniFile.h"

namespace KGI_TW_Der_Utility{   

class IniSettings {
public:
    IniSettings( int readBufferSize = 1024);    
    virtual ~IniSettings();
    
    bool Load( const std::string& filename );
    
    IniFile& GetIniFile();
    const IniFile& GetIniFile() const;
private:
    std::string m_Filename;    
    int m_ReadBufferSize;
    static const char m_PtnSection[50];
    static const char m_PtnKeyValue[50];
    RegexUtil m_RegSection;
    RegexUtil m_RegKeyValue;
    
    IniFile m_IniFile;
    
    IniSection* m_pCrtSection;
    
private:
    /// Parse ini line
    ///
    /// \param pBuffer buffer address
    /// \param length the length of buffers
    /// \return the last index + 1 of the used data
    int Parse( const char *pBuffer, int length );
   

};

}

#endif	/* UTILITY_INISETTINGS_H */

