/* 
 * File:   RegexUtil.h
 * Author: 0007989
 *
 * Created on 2014年3月17日, 下午 5:59
 */

#ifndef _OS_LINUX_
#define _Enable_TR1_Regex_
#endif

#ifndef UTILITY_REGEXUTIL_H
#define	UTILITY_REGEXUTIL_H

#ifdef _OS_LINUX_
#include <regex.h>
#endif
#include <string>
#include <regex>
#include <vector>
#include <mutex>

namespace KGI_TW_Der_Utility
{   

/// \brief Wrapper class for regex functions
class RegexUtil {
public:
    RegexUtil();
    RegexUtil(const char *pPattern );
    RegexUtil(const RegexUtil& orig);
    virtual ~RegexUtil();
    
    /// \brief compile with the given pattern
    /// \param pPattern pattern for matching
    /// \return return the regcompile() result code, 0 means success.
    int Compile( const char *pPattern );
    /// \return return the regmatch() result code. 0 means success. 
    //int Match( const char *pData, int nMatch, regmatch_t *pResult ) const;
    int Match( const char* pData, int nMatch, std::vector<std::string>& result ) const;
private:
    bool m_IsCompiled;          //!< is regular express is ready for using    
    std::string m_Pattern;      //!< current pattern
#ifdef _Enable_TR1_Regex_
    std::regex r;
#else
    regex_t m_RegHandle;        //!< regex handle
#endif
    mutable std::mutex m_lock;
};

}

#endif	/* UTILITY_REGEXUTIL_H */

