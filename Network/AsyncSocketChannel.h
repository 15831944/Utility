/* 
 * File:   AsyncSocketChannel.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年6月17日, 下午 6:01
 */

#ifndef UTILITY_ASYNCSOCKETCHANNEL_H
#define	UTILITY_ASYNCSOCKETCHANNEL_H

#include "SocketChannel.h"
#include "../Collect/BlockingQueue.h"

namespace KGI_TW_Der_Utility{  

class AsyncSocketChannel : public SocketChannel{
    struct SendData{
        ~SendData(){ delete[] pBuffer; }
        char *pBuffer;
        int BufferSize;
    };
public:
    AsyncSocketChannel( int socketID, int maxPendingData = 5000);
    virtual ~AsyncSocketChannel();
    
    virtual int Write( const char *pBuffer, int bufferSize );
    
    void SetMaxPendingData( int maxPendingData ) { m_MaxPendingData = maxPendingData; }
    int GetMaxPendingData() const { return m_MaxPendingData; }
protected:
    int m_MaxPendingData;               //!< 最大可等待傳送的資料量
    BlockingQueue<std::shared_ptr<SendData>> m_DataQueue;
protected:
    void AsyncSendData();
};

}

#endif	/* UTILITY_ASYNCSOCKETCHANNEL_H */

