/* 
 * File:   StringUtil.h
 * Author: 0007989
 *
 * Created on 2014年3月7日, 下午 12:07
 */

#ifndef UTILITY_STRINGUTIL_H
#define	UTILITY_STRINGUTIL_H

#include <stdarg.h>
#include <algorithm>
#include <string>
#include <list>
#include <functional>
#include <cctype>

namespace KGI_TW_Der_Utility
{   

/// String Utility
///
///
class StringUtil {
public:
    /// Constructor
    StringUtil();
    /// Copy Constructor
    ///
    /// \param orig the source for copying
    StringUtil(const StringUtil& orig);
    /// Destructor
    virtual ~StringUtil();
    /// Format string
    ///
    /// \param pFmt the foramt pattern   
    void Format( const char *pFmt, ... );
    /// Get the formated data
    ///
    /// \return the formated string buffer
    const char *GetData();
    /// Get the length of string buffer
    ///
    /// \return the length of string buffer
    int GetLength();
    
    static char *StringFormat( int &defaultSize, const char *pFmt, ...  );
    static char *StringFormatImpl( int &defaultSize, const char *pFmt, va_list &arg  );
    
    static void StringFormatEx( std::string& result, const char* pFmt, va_list& ap, int defaultSize = 100 );
    
    static char *StringFormatImpl( char* pBuffer, int& defaultSize, const char* pFmt, va_list& arg );
    
    static char *BinaryToASCII( const char *pStr, int size, int &length );
    static char *ASCIIToBinary( const char *pASCII, int size, int &length );
    
    static char *GB2BIG5( const char *pGB, int gbLength, int &length);
    static char *BIG52GB( const char *pBIG5, int big5Length, int &length);
    static char *TransCodePage( const char *pSrc, int srcSize, const char *pSrcCode, const char *pDstCode, int &dstSize );

    // ref : http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
    // trim from start
    static inline std::string &ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
            return s;
    }

    // trim from end
    static inline std::string &rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
            return s;
    }

    // trim from both ends
    static inline std::string &trim(std::string &s) {
            return ltrim(rtrim(s));
    }
    
    static void Split(std::list<std::string> &tokens, const std::string &text, const char sep);
    static bool SplitDualString( const std::string& orig, char spliter, std::string& str1, std::string& str2 );
    
    static void Copy( char* pDst, int lenDst, const char *pSrc, int lenSrc );

private:
    char *m_pData;
    int m_Length;
};

}

#endif	/* UTILITY_STRINGUTIL_H */

