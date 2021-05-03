/* 
 * File:   MovingBuffer.h
 * Author: 0007989
 *
 * Created on 2014年4月29日, 下午 5:37
 */

#ifndef UTILITY_MOVINGBUFFER_H
#define	UTILITY_MOVINGBUFFER_H

#include "MacroRecipe.h"

namespace KGI_TW_Der_Utility{   

class MovingBuffer {
public:
    MovingBuffer( int totalSize );
    MovingBuffer(const MovingBuffer& orig);
    virtual ~MovingBuffer();
    
    MovingBuffer& operator=( const MovingBuffer& orig );
    
    char *GetBuffer(){ return m_pBuffer; }
    const char* GetBuffer() const { return m_pBuffer; }
    int GetTotalSize() const{ return m_TotalSize; }
    /// \brief after caching buffer, must GetBuffer() again, because it might extend memory
    void CacheData( const char *pSrc, int length );
    int GetPosition() const{ return m_Position; }
    void SetPosition( int pos ){ m_Position = pos; }
private:
    char *m_pBuffer;
    int m_TotalSize;
    int m_Position;

};

}
#endif	/* UTILITY_MOVINGBUFFER_H */

