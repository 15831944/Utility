/* 
 * File:   WorkingBuffer.cpp
 * Author: 0007989
 * 
 * Created on 2014年2月6日, 下午 1:19
 */

#include "WorkingBuffer.h"

#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <assert.h>

namespace KGI_TW_Der_Utility
{   

WorkingBuffer::WorkingBuffer(int defaultSize ) : m_MaxSize(defaultSize),m_UsedSize(0) {
    m_pData = new char[m_MaxSize];
    ::memset( m_pData, 0, sizeof( char ) * m_MaxSize );
}

WorkingBuffer::WorkingBuffer(const WorkingBuffer& orig) {      
    // TODO
}

WorkingBuffer::~WorkingBuffer() {    
    
    if( m_pData != NULL)
        delete[] m_pData;
}

char *WorkingBuffer::GetData(){
    m_DataLocker.lock();
    return m_pData;
}

void WorkingBuffer::ReleaseData(){
    m_DataLocker.unlock();
}

///
/// \param usedBytes The data that are consumed and want to be discarded from buffer
///
void WorkingBuffer::ChangeBufferUsage( int usedBytes ){
    
    std::unique_lock<std::mutex> al( m_DataLocker );    
    
    if( usedBytes <= m_UsedSize ) //the usedBytes can not be larger than m_UsedSize
    {
        ::memcpy( m_pData, m_pData + usedBytes, m_UsedSize - usedBytes );        
        m_UsedSize -= usedBytes;        
        ::memset( m_pData + m_UsedSize, 0, sizeof(char) * usedBytes ); // clear released buffer
    }    
}

int WorkingBuffer::GetMaxSize() const{
    return m_MaxSize;
}

int WorkingBuffer::GetUsedSize() const{
    return m_UsedSize;
}

void WorkingBuffer::SetUsedSize( int usedSize ){
    if( usedSize >= 0 && usedSize <= m_MaxSize ){
        m_UsedSize = usedSize;
    }
}

int WorkingBuffer::GetAvailabeSize() const{
    return m_MaxSize - m_UsedSize;
}

#ifdef _WorkingBufferEx_
WorkingBuffer::ErrorCode WorkingBuffer::CacheData( const char *pData, int size ){
    if( size + m_UsedSize >= m_MaxSize )
    {
        // Extend memory
        try
        {
            if( Extend( (m_UsedSize + size) * 2) != ErrorCode::Success )
                throw;
        }
        catch( ... )
        {
            return ErrorCode::MemoryNotEnouth; // memory is not enough
        }
    }
    
    ::memcpy( m_pData + m_UsedSize, pData, size );
    m_UsedSize += size;
    
    return ErrorCode::Success;
}
#else
WorkingBuffer::ErrorCode WorkingBuffer::CacheData( const char *pData, int size ){
    if( size + m_UsedSize >= m_MaxSize )
    {
        // Extend memory
        try
        {
            char *pOrg = m_pData;
            int orgSize = m_UsedSize;

            m_MaxSize = (m_UsedSize + size) * 2;
            m_pData = new char[m_MaxSize];
            ::memcpy( m_pData, pOrg, orgSize );
            ::memset( m_pData + orgSize, 0, m_MaxSize - orgSize );
            delete[] pOrg;
        }
        catch( ... )
        {
            return ErrorCode::MemoryNotEnouth; // memory is not enough
        }
    }
    
    ::memcpy( m_pData + m_UsedSize, pData, size );
    m_UsedSize += size;
    
    return ErrorCode::Success;
}
#endif

WorkingBuffer::ErrorCode WorkingBuffer::Extend( int extendSize ){
    try
    {
        char *pNewBuffer = (char *)::realloc( m_pData, m_MaxSize + extendSize );
        if( pNewBuffer == NULL )
        {
            // extend by allocate and copy
            char *pOrg = m_pData;
            int orgSize = m_UsedSize;

            m_MaxSize += extendSize;
            m_pData = new char[m_MaxSize];
            ::memcpy( m_pData, pOrg, orgSize );
            ::memset( m_pData + orgSize, 0, m_MaxSize - orgSize );
            delete[] pOrg;
            return ErrorCode::Success;
        }
        else
        {
            m_pData = pNewBuffer;
            m_MaxSize += extendSize;
            return ErrorCode::Success;
        }
    }
    catch( ... )
    {
        return ErrorCode::ExtendMemFailed;
    }
}

}
