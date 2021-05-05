/* 
 * File:   MovingBuffer.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月29日, 下午 5:37
 */

#include "MovingBuffer.h"

#include <stdlib.h>
#include <string.h>

namespace ProgLab{ namespace Utility{   

MovingBuffer::MovingBuffer( int totalSize ) {
    m_TotalSize = totalSize;
    m_pBuffer = (char *)::malloc( totalSize );
    m_Position = 0;
}

MovingBuffer::MovingBuffer(const MovingBuffer& orig) {
}

MovingBuffer::~MovingBuffer() {
    free( m_pBuffer );
}

MovingBuffer& MovingBuffer::operator=( const MovingBuffer& orig ){
    if( this != &orig )
    {
        free( m_pBuffer );
        m_TotalSize = orig.m_TotalSize;
        m_pBuffer = (char *)::malloc( m_TotalSize );
        memcpy( m_pBuffer, orig.m_pBuffer, m_TotalSize );
        m_Position = orig.m_Position;
    }
    return *this;
}

void MovingBuffer::CacheData( const char *pSrc, int length ){
    
    if( m_Position + length > m_TotalSize )
    {
        // Extend
        char *pOrgBuffer = m_pBuffer; // backup original buffer, in case the realloc is failed
        int orgSize = m_TotalSize;
        m_TotalSize += m_TotalSize;     // Extend size two times        
        m_pBuffer = (char *)::realloc( m_pBuffer, m_TotalSize );
        if( m_pBuffer == NULL ) // Extend failed
        {
            m_pBuffer = (char *)::malloc( m_TotalSize );
            if( m_pBuffer != NULL )
            {
                memcpy( m_pBuffer, pOrgBuffer, orgSize );
            }
        }
    }
       
    memcpy( m_pBuffer + m_Position, pSrc, length );
    m_Position += length;
    
}

}