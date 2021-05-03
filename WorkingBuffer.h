/* 
 * File:   WorkingBuffer.h
 * Author: 0007989
 *
 * Created on 2014年2月6日, 下午 1:19
 */

#define _WorkingBufferEx_

#ifndef UTILITY_WORKINGBUFFER_H
#define	UTILITY_WORKINGBUFFER_H

#include <mutex>

namespace KGI_TW_Der_Utility
{   

class WorkingBuffer {
public:
    enum class ErrorCode
    {
        Success = 0,
        MemoryNotEnouth = 1,
        ExtendMemFailed = 2,
    };
public:
    WorkingBuffer( int defaultSize );
    WorkingBuffer(const WorkingBuffer& orig);
    virtual ~WorkingBuffer();
    
    /// \brief get working buffer data. The data will be locked. After using the data, must invoke ReleaseData() to release locker.
    /// \return the working buffer data.
    char *GetData();
    /// \brief release the previously requested working buffer.
    void ReleaseData();
    /// \brief moving the used position of buffer
    /// \param the size for adding up the index
    void ChangeBufferUsage( int usedBytes );
    int GetMaxSize() const;
    int GetUsedSize() const;
    void SetUsedSize( int usedSize );
    int GetAvailabeSize() const;
    /// \brief put the specified buffer after the current index. If the buffer is not enouth, it will automatically extend buffer size.
    /// \param pData the data for caching into this working buffer
    /// \param the total size of will be cached data
    /// \return 
    ErrorCode CacheData( const char *pData, int size );
    ErrorCode Extend( int extendSize );
private:
    char *m_pData;              //!< buffer
    int m_MaxSize;              //!< Maximu size of buffer
    int m_UsedSize;             //!< Currently used size of buffer
    std::mutex m_DataLocker;        //!< Control the access of m_pData in multi-threading

};

}

#endif	/* UTILITY_WORKINGBUFFER_H */

