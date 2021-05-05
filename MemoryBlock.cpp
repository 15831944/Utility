/* 
 * File:   MemoryBlock.cpp
 * Author: 0007989
 * 
 * Created on 2014年10月14日, 下午 4:38
 */

#include "MemoryBlock.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

namespace ProgLab{ namespace Utility{  

MemoryBlock::MemoryBlock( int size ):m_Size(size),m_pBuffer(NULL){
    if( m_Size > 0 )
        Extend( m_Size );
}

MemoryBlock::MemoryBlock( const MemoryBlock& orig ){
    *this = orig;
}
    
MemoryBlock::~MemoryBlock(){
    delete[] m_pBuffer;
}

MemoryBlock& MemoryBlock::operator=( const MemoryBlock& orig ){
    if( this != &orig ){
        delete[] m_pBuffer;
        m_Size = 0;
        Extend( orig.m_Size );
        memcpy( m_pBuffer, orig.m_pBuffer, m_Size * sizeof(char) );
    }
    return *this;
}

bool MemoryBlock::Extend( int newSize ){
    if( m_pBuffer == NULL ){
        m_Size = newSize;
        m_pBuffer = (char *)malloc( m_Size * sizeof(char) );
        memset( m_pBuffer, 0, m_Size * sizeof(char) );
    }else{
        //assert( newSize > m_Size );
        if( newSize > m_Size ){
            int orgSize = m_Size;
            m_Size = newSize;
            m_pBuffer = (char *)realloc( m_pBuffer, m_Size * sizeof(char) );
            memset( m_pBuffer + orgSize, 0, sizeof(char) * (m_Size - orgSize) );
        }else
            return false;
    }
    return true;
}

bool MemoryBlock::StringFormat( const char* pFmt, ... ){
    va_list ap;
    va_start(ap,pFmt);
    bool result = StringFormatImpl( pFmt, ap );
    va_end(ap);
    
    return result;
}
    
#ifdef _OS_LINUX_
bool MemoryBlock::StringFormatImpl( const char* pFmt, va_list& ap ){
    try{
        va_list orgAP;
        va_copy(orgAP, ap );        
        
        // calculate need size 
        int needSize = vsnprintf( m_pBuffer, m_Size, pFmt, orgAP);
        if( needSize > m_Size ){
            Extend( needSize + 1 );
            
            needSize = vsnprintf( m_pBuffer, m_Size, pFmt, ap);
        }
        return true;
    }
    catch(...){        
        return false;
    }
}
#else // _OS_WIN_
bool MemoryBlock::StringFormatImpl( const char* pFmt, va_list& ap ){
    try{
        va_list orgAP;
        va_copy(orgAP, ap );        
        
        // calculate need size 
        int needSize = _vsnprintf_s( m_pBuffer, m_Size, m_Size, pFmt, orgAP);
        if( needSize > m_Size ){
            Extend( needSize + 1 );
            
            needSize = _vsnprintf_s( m_pBuffer, m_Size,m_Size, pFmt, ap);
        }
        return true;
    }
    catch(...){        
        return false;
    }
}
#endif

}