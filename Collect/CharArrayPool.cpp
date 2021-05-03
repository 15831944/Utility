/* 
 * File:   CharArrayPool.cpp
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 * 
 * Created on 2015年8月5日, 下午 1:42
 */

#include "CharArrayPool.h"
#include <memory.h>

namespace KGI_TW_Der_Utility{
    
CharArrayPool::CharArrayPool(int count, int lengthPerItem):m_Count(count),m_LengthPerItem(lengthPerItem){
    m_pPool = new char[ m_Count * m_LengthPerItem ];
    m_pItems = new char*[m_Count];
    memset( m_pPool, 0, sizeof( m_Count * m_LengthPerItem ) );
    for( int i = 0; i < m_Count; ++i ){
        m_pItems[i] = m_pPool + i * m_LengthPerItem;
    }
}

CharArrayPool::~CharArrayPool(){
    delete[] m_pPool;
    delete[] m_pItems;
}

void CharArrayPool::Clear(){
    memset( m_pPool, 0, sizeof( m_Count * m_LengthPerItem ) );
}

void CharArrayPool::Copy(int itemIndex, const char* pSrc, int length){
    if( itemIndex >= 0 && itemIndex < m_Count && length <= m_LengthPerItem ){
        memcpy( m_pItems[itemIndex], pSrc, length );
    }
}

}