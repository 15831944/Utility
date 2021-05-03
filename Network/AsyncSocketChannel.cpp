/* 
 * File:   AsyncSocketChannel.cpp
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 * 
 * Created on 2015年6月17日, 下午 6:01
 */

#include "AsyncSocketChannel.h"

#include <thread>
#include <memory.h>

namespace KGI_TW_Der_Utility{

AsyncSocketChannel::AsyncSocketChannel(int socketID, int maxPendingData) : SocketChannel(socketID),m_MaxPendingData(maxPendingData) {
    std::thread tSendData( &AsyncSocketChannel::AsyncSendData, this );
    tSendData.detach();
}

AsyncSocketChannel::~AsyncSocketChannel() {
    m_DataQueue.Abort();
}

int AsyncSocketChannel::Write( const char *pBuffer, int bufferSize ){
    if( m_DataQueue.Size() >= m_MaxPendingData )
        return 0; // buffer full
    
    if( pBuffer == NULL || bufferSize == 0 )
        return 0;
    
    std::shared_ptr<SendData> sendData( new SendData() );
    sendData->BufferSize = bufferSize;
    sendData->pBuffer = new char[bufferSize];
    memcpy( sendData->pBuffer, pBuffer, bufferSize );
    
    m_DataQueue.Push( sendData );
    
    return bufferSize;
}

void AsyncSocketChannel::AsyncSendData(){
    while(true){
        try{
            std::shared_ptr<SendData> sendData = m_DataQueue.Pop();
            //printf("AsyncSocketChannel::AsyncSendData() reamin(%d)\r\n", m_DataQueue.Size() );
            if( sendData != NULL && sendData->BufferSize > 0 && sendData->pBuffer != NULL ){
                if( m_SocketID != 0 ){
                    int sentBytes = SocketChannel::Write( sendData->pBuffer, sendData->BufferSize );                    
                }
            }
        }catch(...){
            break; // exit loop
        }
    }
}

}