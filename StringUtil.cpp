/* 
 * File:   StringUtil.cpp
 * Author: 0007989
 * 
 * Created on 2014年3月7日, 下午 12:07
 */

#include "StringUtil.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _OS_LINUX_
#ifdef _OPENCC_
#include <iconv.h>
#include <opencc/opencc.h>
#endif
#else // WIN32
#include <tchar.h>
#endif
#include <errno.h>

namespace KGI_TW_Der_Utility{   

StringUtil::StringUtil():m_pData(NULL),m_Length(100) {
}

StringUtil::StringUtil(const StringUtil& orig) {
}

StringUtil::~StringUtil() {
    delete[] m_pData;
}

void StringUtil::Format( const char *pFmt, ... )
{
    if( m_pData != NULL ){
        delete[] m_pData;
        m_Length = 100;
    }
    
    va_list ap;
    va_start(ap,pFmt);
    m_pData = StringFormatImpl( m_Length, pFmt, ap);
    va_end(ap);
        
}

const char *StringUtil::GetData(){
    return m_pData;
}

int StringUtil::GetLength(){
    return m_Length;
}

#ifdef _OS_LINUX_
char *StringUtil::StringFormatImpl( char* pBuffer, int& defaultSize, const char* pFmt, va_list& arg ){
    
    bool isAllocateBuffer = false;
    // Check and init buffer
    if( defaultSize <= 0 || pBuffer == NULL ){
        if( defaultSize <= 0 )
            defaultSize = 100;
        pBuffer = new char[defaultSize];        
        isAllocateBuffer = true;
    }
    
    int needSize = vsnprintf( pBuffer, defaultSize, pFmt, arg);    
    needSize++; // count in the null character
    if( needSize > defaultSize ){
        defaultSize = needSize;
        if( isAllocateBuffer )
            delete[] pBuffer;
        return NULL;
    }else{
        defaultSize = needSize-1;
        return pBuffer;
    }    
}

void StringUtil::StringFormatEx( std::string& result, const char* pFmt, va_list& ap, int defaultSize ){
    try{
        va_list orgAP;
        va_copy(orgAP, ap );
        if( defaultSize <= 0 ){
            defaultSize = 100;
        }
            
        result.resize( defaultSize );        
        
        // calculate need size 
        int needSize = vsnprintf( &result[0], defaultSize + 1, pFmt, orgAP);
        if( needSize >= defaultSize ){ // needSize does not contain the null character thus if needSize == defaultSize, the buffer still need one more byte 
            defaultSize = needSize;
            result.resize( defaultSize );
            
            needSize = vsnprintf( &result[0], defaultSize + 1, pFmt, ap);
        }else
            result.resize( needSize );
    }catch(...){        
    }
}

char *StringUtil::StringFormatImpl( int &defaultSize, const char *pFmt, va_list &ap  ){

    try{
        va_list orgAP;
        va_copy(orgAP, ap );
        if( defaultSize <= 0 )
            defaultSize = 100;
        char *pBuffer = new char[defaultSize+1];
        memset( pBuffer, 0, defaultSize+1 );
        
        // calculate need size 
        int needSize = vsnprintf( pBuffer, defaultSize, pFmt, orgAP);
        if( needSize >= defaultSize ){ // needSize does not contain the null character thus if needSize == defaultSize, the buffer still need one more byte 
            delete[] pBuffer;
            defaultSize = needSize;
            
            pBuffer = new char[defaultSize+1];
            memset( pBuffer, 0, defaultSize+1);
            
            needSize = vsnprintf( pBuffer, defaultSize + 1, pFmt, ap);
        }else
            defaultSize = needSize;
        
        return pBuffer;
    }catch(...){
        defaultSize = 0;
        return NULL;
    }
}
#else
char *StringUtil::StringFormatImpl( int &defaultSize, const char *pFmt, va_list &ap  ){
	va_list orgAP;	
	va_copy(orgAP, ap);

	int needSize = _vscprintf(pFmt, orgAP); // calculate required size
	needSize += 10;
    char *pBuffer = (char *)malloc( needSize +1);   
	::memset(pBuffer, 0, needSize + 1);
	needSize = _vsnprintf_s(pBuffer, needSize, needSize, pFmt, ap);
	defaultSize = needSize;
    return pBuffer;
}

void StringUtil::StringFormatEx( std::string& result, const char* pFmt, va_list& ap, int defaultSize ){
    try{
        va_list orgAP;
        va_copy(orgAP, ap );
        if( defaultSize <= 0 ){
            defaultSize = 100;
        }
            
        result.resize( defaultSize );        
        
        // calculate need size 
        int needSize = _vsnprintf_s( &result[0], defaultSize + 1, pFmt, orgAP);
        if( needSize >= defaultSize ){ // needSize does not contain the null character thus if needSize == defaultSize, the buffer still need one more byte 
            defaultSize = needSize;
            result.resize( defaultSize );
            
            needSize = _vsnprintf_s( &result[0], defaultSize + 1, pFmt, ap);
        }else
            result.resize( needSize );
    }
    catch(...){        
    }
}

char *StringUtil::StringFormatImpl( char* pBuffer, int& defaultSize, const char* pFmt, va_list& arg ){
    
    bool isAllocateBuffer = false;
    // Check and init buffer
    if( defaultSize <= 0 || pBuffer == NULL ){
        if( defaultSize <= 0 )
            defaultSize = 100;
        pBuffer = new char[defaultSize];        
        isAllocateBuffer = true;
    }
    
    int needSize = _vsnprintf_s( pBuffer, 1, defaultSize, pFmt, arg);
    needSize++; // count in the null character
    if( needSize > defaultSize ){
        defaultSize = needSize;
        if( isAllocateBuffer )
            delete[] pBuffer;
        return NULL;
    }else{
        defaultSize = needSize-1;
        return pBuffer;
    }
    
}
#endif


char *StringUtil::StringFormat( int &defaultSize, const char *pFmt, ...  ){    
    va_list ap;
    va_start(ap,pFmt);
    char *pResult = StringFormatImpl( defaultSize, pFmt, ap );
    va_end(ap);
    
    return pResult;
}


/// Convert binary data to ASCII string
///
/// \param pStr the original binary data
/// \param size the length of original binary data
/// \param length return the length of converted data
/// \return converted data
char *StringUtil::BinaryToASCII( const char *pStr, int size, int &length ){   
    static char mapTable[] = {'A','B','C','D','E','F' };
    static int shiftBits[2] = { 0, 4 };
    length = size*2+1;
    char *pResult = new char[length];
    ::memset( pResult, 0, sizeof(char) * length );    
    for( int c = 0; c < size; ++c ){      
        for( int ascii = 0; ascii < 2; ++ascii ){
            char crt = pStr[c];
            //int shiftBits = 4 * ascii;
            crt = crt >> shiftBits[ascii];
            crt &= 0x0F;
            if( crt >= 10 ){
                crt %= 10;
                crt = mapTable[(int)crt];
            }else
                crt += 0x30;           
            pResult[2*c + (ascii+1)%2] = crt;
        }        
    }    
    
    return pResult;
}

/// Convert ASCII string to binary data
///
/// \param pASCII the original ASCII string
/// \param size the lenght of original ASCII string
/// \param length return the length of converted data
/// \return the converted data
char *StringUtil::ASCIIToBinary( const char *pASCII, int size, int &length ){
    length = size;
    char *pResult = new char[size];
    ::memset( pResult, 0, sizeof(char) * size );
    int idxASCII =0, idxByte = 0, idxResult = 0;
    for( idxASCII = 0; idxASCII < size; ++idxASCII ){
        char crt = *(pASCII + idxASCII);
        if( crt == ' ')
            continue;
        
        if( crt >= 'A' && crt <= 'F'){
            crt -= 'A';
            crt += 10;
        }else if ( crt >= 'a' && crt <= 'f'){
            crt -= 'a';
            crt += 10;
        }else{
            crt -= 0x30;
        }
        
        if( idxByte == 0 )
            crt = crt << 4;

        *(pResult + idxResult) |= crt;
        
        idxByte++;
        if( idxByte >= 2 ){
            idxByte = 0;            
            idxResult++;
        }
    }
    
    length = idxResult;
    
    return pResult;
    
}

char *StringUtil::GB2BIG5( const char *pGB, int gbLength, int &length){
    
    int utf8Length = 0;
    char *pUTF8 = TransCodePage( pGB, gbLength, "GB2312", "UTF-8", utf8Length ); // trans src to utf8
        
#ifdef _OPENCC_
    opencc_t od = opencc_open(OPENCC_DEFAULT_CONFIG_SIMP_TO_TRAD); 
    if( od != (opencc_t)-1 ){
        char * chsutf8 = opencc_convert_utf8(od, pUTF8, strlen(pUTF8));  //     
        delete[] pUTF8;  
        pUTF8 = chsutf8;
        
        opencc_close(od);
    }    
#endif
        
    char* pBIG5 = TransCodePage( pUTF8, strlen( pUTF8 ), "UTF-8", "BIG-5", length );    
    delete[] pUTF8;
    
    return pBIG5;
}
    
char *StringUtil::BIG52GB( const char *pBIG5, int big5Length, int &length){
       
    int utf8Length = 0;
    char *pUTF8 = TransCodePage( pBIG5, big5Length, "BIG-5", "UTF-8", utf8Length ); // trans src to utf8    
    
#ifdef _OPENCC_
    opencc_t od = opencc_open(OPENCC_DEFAULT_CONFIG_TRAD_TO_SIMP); 
    if( od != (opencc_t)-1){
        char * chsutf8 = opencc_convert_utf8(od, pUTF8, strlen(pUTF8));  //     
        delete[] pUTF8;
        pUTF8 = chsutf8;
        
        opencc_close( od );
    }    
#endif
    char* pGB = TransCodePage( pUTF8, strlen( pUTF8 ), "UTF-8", "GB2312", length );        
    delete[] pUTF8;
    
    return pGB;

}

char *StringUtil::TransCodePage( const char *pSrc, int srcSize, const char *pSrcCode, const char *pDstCode, int &dstSize ){
#if defined(_OS_LINUX_)
#if defined(_OPENCC)
    iconv_t ip = iconv_open( pDstCode,pSrcCode );
    
    size_t dstIndex = srcSize * 2;
    size_t srcIndex = srcSize;
    int orgSize = dstIndex;
        
    char *pSrcTmp = (char *)pSrc;
    char *result = new char[dstIndex];
    memset( result, 0, dstIndex );
    
    if( ip != (iconv_t)(-1) ){
        char *pDst = result;    
        size_t r = iconv( ip, &pSrcTmp, &srcIndex, &pDst, &dstIndex );    
        //printf( "[%s => %s] R=%d,SrcIndex=%d,DstIndex=%d,errno=%d,%s\r\n", pSrcCode, pDstCode, r, srcIndex, dstIndex,errno, result );

        iconv_close( ip );

        dstSize = orgSize - dstIndex;
    }else{
        strncpy( result, pSrc, srcSize );
    }
    return result;
#else // No opencc
    char *result = new char[srcSize];
    memset(result, 0, srcSize);
    strncpy(result, pSrc, srcSize);
    return result;
#endif
    
#else // WIN32
    char *result = new char[srcSize];
    memset(result, 0, srcSize);
    strncpy_s(result, srcSize, pSrc, srcSize);
    return result;
#endif
}

bool StringUtil::SplitDualString( const std::string& orig, char spliter, std::string& str1, std::string& str2 ){
    size_t pos = orig.find( spliter );
    if( pos != std::string::npos && pos > 0 && pos < orig.size() ){
        str1 = orig.substr( 0, pos );
        str2 = orig.substr( pos + 1, orig.size() - pos - 1 );
        return true;
    }else
        return false;
}

void StringUtil::Split(std::list<std::string> &tokens, const std::string &text, const char sep) {
    //unsigned int start = 0, end = 0;
    size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
}

void StringUtil::Copy( char* pDst, int lenDst, const char *pSrc, int lenSrc ){
    int length = lenDst > lenSrc ? lenSrc : lenDst;
    memcpy( pDst, pSrc, length );
    if( length < lenDst ){
        pDst[length+1] = '\0';
    }
}

}