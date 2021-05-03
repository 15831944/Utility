/* 
 * File:   MemoryBlock.h
 * Author: 0007989
 *
 * Created on 2014年10月14日, 下午 4:38
 */

#ifndef UTILITY_MEMORYBLOCK_H
#define	UTILITY_MEMORYBLOCK_H

#include <stdarg.h>

namespace KGI_TW_Der_Utility{  

class MemoryBlock{
public:
    MemoryBlock( int size = 0 );
    MemoryBlock( const MemoryBlock& orig );
    ~MemoryBlock();
    
    MemoryBlock& operator=( const MemoryBlock& orig );
    
    bool Extend( int newSize );
    
    bool StringFormat( const char* pFmt, ... );
    bool StringFormatImpl( const char* pFmt, va_list& ap);
    
    char *Get() const { return m_pBuffer; }
    int Size() const { return m_Size; }
private:
    int         m_Size;
    char*       m_pBuffer;    
};

}
#endif	/* UTILITY_MEMORYBLOCK_H */

