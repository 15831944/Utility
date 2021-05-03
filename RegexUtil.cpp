/* 
 * File:   RegexUtil.cpp
 * Author: 0007989
 * 
 * Created on 2014年3月17日, 下午 5:59
 */

#include "RegexUtil.h"

#include <string.h>
#include <memory>
#include <assert.h>
//#include <bits/unique_ptr.h>

namespace KGI_TW_Der_Utility
{   


RegexUtil::RegexUtil(){
    m_IsCompiled = false;
#ifndef _Enable_TR1_Regex_
    ::memset( &m_RegHandle, 0, sizeof(m_RegHandle) );
#endif
    
}

RegexUtil::RegexUtil(const char *pPattern ) {
    m_IsCompiled = false;
    if( pPattern == NULL )
        return;        
    
    int rc = Compile( pPattern );
    if( rc > 0 )
        throw "RegexUtil() compile pattern failed, RC=" + rc;
}

RegexUtil::RegexUtil(const RegexUtil& orig){    
}

RegexUtil::~RegexUtil() {
#ifndef _Enable_TR1_Regex_
    ::regfree( &m_RegHandle );
#endif
}

int RegexUtil::Compile(const char *pPattern ){
    m_Pattern = pPattern;
    
#ifdef _Enable_TR1_Regex_
    r.assign( pPattern, std::regex::extended );
    int rc = 0;
#else
    int rc = ::regcomp(&m_RegHandle, pPattern, REG_EXTENDED );
#endif    
    m_IsCompiled = rc == 0;
    return rc;
}
/*
int RegexUtil::Match( const char *pData, int nMatch, regmatch_t *pResult ) const{    
    if( !m_IsCompiled )
        return REG_BADPAT;
    
    int rc = ::regexec( &m_RegHandle, pData, nMatch, pResult, 0 );
    
    return rc;
}
*/
int RegexUtil::Match( const char* pData, int nMatch, std::vector<std::string>& result ) const{
    try{
        std::lock_guard<std::mutex> al( m_lock );
    
        result.clear();
    
	if (!m_IsCompiled)
            return 2; // REG_BADPAT;
    
#ifdef  _Enable_TR1_Regex_
    int rc = 0;
	std::string searchStr(pData);
    //std::smatch sm;
	//std::regex_match(std::string(pData), sm, r);
	std::cmatch sm; // for VS2015
	const char *first = pData;
	const char *last = pData + strlen(first);
    std::regex_match( first, last, sm, r );	
    if( sm.size() <= 0 )
        rc = 2; //REG_BADPAT;
    else
        result.assign( sm.begin(), sm.end() );
#else    
    int len = strlen( pData );
    regmatch_t* pResult = new regmatch_t[nMatch];    
    int rc = ::regexec( &m_RegHandle, pData, nMatch, pResult, 0 );
    if( rc == 0 ){
        for( int i = 0; i < nMatch; ++i ){
            if( pResult[i].rm_so < 0 || pResult[i].rm_eo < 0 || pResult[i].rm_so > len || pResult[i].rm_eo > len || (pResult[i].rm_eo - pResult[i].rm_so) <= 0 )
                result.push_back( std::string("") );
            else
                result.push_back( std::string( pData + pResult[i].rm_so, pResult[i].rm_eo - pResult[i].rm_so ) );
        }
    }
//    
//    std::unique_ptr<regmatch_t[]> mResult( new regmatch_t[nMatch] );
//    
//    int rc = ::regexec( &m_RegHandle, pData, nMatch, mResult.get(), 0 );
//    if( rc == 0 ){
//        for( int i = 0; i < nMatch; ++i ){
//            result.push_back( std::string( pData + mResult[i].rm_so, mResult[i].rm_eo - mResult[i].rm_so ) );
//        }
//    }
    delete[] pResult;
#endif
        return rc;
    }catch(...){
        return -1;
    }   
}


}