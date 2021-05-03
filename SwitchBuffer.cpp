/* 
 * File:   SwitchBuffer.cpp
 * Author: 0007989
 * 
 * Created on 2014年10月23日, 下午 4:17
 */

#include "SwitchBuffer.h"
#include <assert.h>
#include <thread>
#include <string.h>

namespace KGI_TW_Der_Utility{
    
#ifdef _SWITCHBUFFER_EX_

SwitchBuffer::SwitchBuffer(int bufferSize , int numOfBuffers ,std::function<void(const char*,int)> notifyFunc ) :
    m_BufferSize(bufferSize),m_NumOfBuffers(numOfBuffers),m_IsTimerRunning(false),m_TimerPeriodMS(1000),m_IsStopTimerNotify(false),m_NotifyBufferFull(notifyFunc) {
    
    for( int i = 0; i < m_NumOfBuffers; ++i ){
        std::shared_ptr<WriteBufferData> newBuffer( new WriteBufferData(m_BufferSize));
        m_BufferQueue.push( newBuffer );
    }
    
    m_CrtBuffer = m_BufferQueue.front();
    m_BufferQueue.pop();
    {        
        if( m_CrtBuffer != NULL ){
            m_CrtBuffer->SetIndex(0);
        }
    }
    
    // start thread to notify receiver that a buffer is full
    std::thread tNotify( std::bind(&SwitchBuffer::NotifyThread, this) );
    tNotify.detach();
}

SwitchBuffer::~SwitchBuffer() {
    m_IsStopTimerNotify = true;
    
    // abort notify thread
    m_NotifyQueue.Abort();
    
    std::lock_guard<std::mutex> al( m_LockCrtBuffer );
    
    if( m_IsTimerRunning ){
        m_TimerThreadStopped.Reset();        
        m_TimerEvent.Set(false); // stop notify thread，因為有設定m_IsStop旗標，所以TimerNotifyThread()不會通知寫入最後的Buffer
        if( m_TimerThreadStopped.Wait(5000) == false ){
            fprintf( stderr, "~SwitchBuffer() wait timer thread timeout\r\n" );        
        }    
    }
    
    // noitfy last data
    if( m_CrtBuffer != NULL ){
        m_NotifyBufferFull( m_CrtBuffer->GetBuffer(), m_CrtBuffer->GetIndex() );
    }
    
    // release buffers
    while( m_BufferQueue.size() > 0 ){            
        m_BufferQueue.pop();
    }      
}

int SwitchBuffer::WriteBuffer( const char* pData, int length ){
    std::lock_guard<std::mutex> al( m_LockCrtBuffer );
    
    while(true){        
        if( m_CrtBuffer != NULL ){            
            if( m_CrtBuffer->GetIndex() + length >= m_CrtBuffer->GetLength() ){                
                DoSwitchBuffer();
            }else{
                memcpy( m_CrtBuffer->GetBuffer() + m_CrtBuffer->GetIndex(), pData, length );
                m_CrtBuffer->SetIndex( m_CrtBuffer->GetIndex() + length );
                return length;
            }
        }else
            return 0;
    }    
}

void SwitchBuffer::Flush(){
    std::lock_guard<std::mutex> al( m_LockCrtBuffer );
    {        
        if( m_CrtBuffer != NULL ){            
            if( m_CrtBuffer->GetIndex() > 0 && m_NotifyBufferFull != NULL )
                m_NotifyBufferFull( m_CrtBuffer->GetBuffer(), m_CrtBuffer->GetIndex() );
            
            m_BufferQueue.push( m_CrtBuffer );
            m_CrtBuffer.reset();
        }
        
        if( m_BufferQueue.size() > 0 ){
            m_CrtBuffer = m_BufferQueue.front(); 
            m_BufferQueue.pop();
            if( m_CrtBuffer != NULL ){
                m_CrtBuffer->SetIndex(0);
            }        
        }
    }
}

void SwitchBuffer::DoSwitchBuffer(){    
    
    if( m_CrtBuffer != NULL ){
        m_NotifyQueue.Push( m_CrtBuffer );  
        m_CrtBuffer.reset();
    }
    
    if( m_BufferQueue.size() > 0 ){
        m_CrtBuffer = m_BufferQueue.front();
        m_BufferQueue.pop();
    }
    
    if( m_CrtBuffer != NULL ){            
        m_CrtBuffer->SetIndex(0);
    }    
}

void SwitchBuffer::NotifyThread(){
    while(true){
        try{
            std::shared_ptr<WriteBufferData> wbData = m_NotifyQueue.Pop();
            if( wbData == NULL)
                continue;
            
            if( wbData->GetBuffer() != NULL && wbData->GetIndex() > 0 ){
                if( m_NotifyBufferFull != NULL )
                    m_NotifyBufferFull( wbData->GetBuffer(), wbData->GetIndex() );                
                {
                    std::lock_guard<std::mutex> al( m_LockCrtBuffer );
                    m_BufferQueue.push( wbData ); // return to queue for next time to be used
                }
            }
        }catch(BQThreadAbort exp){            
            break;
        }catch(...){            
        }
    }
}

void SwitchBuffer::TimerNotifyThread(){
    while(true){
        if( m_TimerEvent.Wait(m_TimerPeriodMS) == false ){
            if( m_IsStopTimerNotify )
                break;
            
            std::lock_guard<std::mutex> al( m_LockCrtBuffer );
            {                
                if( m_CrtBuffer != NULL ){  
                    if( m_CrtBuffer->GetIndex() > 0 ){
                        DoSwitchBuffer();
                    }
                }
            }
            
        }else
            break;
    }
    
    m_TimerThreadStopped.Set(false);
}

void SwitchBuffer::SetTimerNotify( int periodMS, bool isEnable ){
    if( isEnable && !m_IsTimerRunning ){
        m_IsTimerRunning = true;
        m_TimerPeriodMS = periodMS;
        std::thread tTimer( std::bind( &SwitchBuffer::TimerNotifyThread, this ) );
        tTimer.detach();
    }if( !isEnable && m_IsTimerRunning ){
        // stop
        m_IsStopTimerNotify = true;
        m_TimerEvent.Set(false);        

        m_IsTimerRunning = false;
    }
}
    
#else

SwitchBuffer::SwitchBuffer(int bufferSize , int numOfBuffers ,std::function<void(const char*,int)> notifyFunc ) :
    m_BufferSize(bufferSize),m_NumOfBuffers(numOfBuffers),m_NotifyBufferFull(notifyFunc),m_IsTimerRunning(false),m_TimerPeriodMS(1000),m_IsStop(false) {
    
    for( int i = 0; i < m_NumOfBuffers; ++i ){
        char* pNewBuffer = new char[bufferSize];
        m_BufferQueue.push( pNewBuffer );
    }
    
    m_pCrtBuffer = m_BufferQueue.front();
    m_BufferQueue.pop();
    m_CrtBufferPosition = 0;
    
    // start thread to notify receiver that a buffer is full
    std::thread tNotify( std::bind(&SwitchBuffer::NotifyThread, this) );
    tNotify.detach();
}

SwitchBuffer::~SwitchBuffer() {
    
    std::lock_guard<std::mutex> al( m_LockCrtBuffer );
    
    m_IsStop = true;
    
    // abort notify thread
    m_NotifyQueue.Abort();
    
    m_TimerEvent.Set(false); // stop notify thread，因為有設定m_IsStop旗標，所以TimerNotifyThread()不會通知寫入最後的Buffer
    
    // noitfy last data
    if( m_CrtBufferPosition > 0 && m_NotifyBufferFull != NULL ){
        m_NotifyBufferFull( m_pCrtBuffer, m_CrtBufferPosition );
    }
    
    {
        // release buffers
        while( m_BufferQueue.size() > 0 ){
            delete[] m_BufferQueue.front();
            m_BufferQueue.pop();
        }
    }
    
    delete[] m_pCrtBuffer;
    m_pCrtBuffer = NULL;
}

int SwitchBuffer::WriteBuffer( const char* pData, int length ){
    std::lock_guard<std::mutex> al( m_LockCrtBuffer );
        
    if( m_pCrtBuffer == NULL ) 
        return 0; // 可能己有人呼叫了Stop()
    
    if( m_CrtBufferPosition + length >= m_BufferSize ){
        DoSwitchBuffer();
    }
    memcpy( m_pCrtBuffer + m_CrtBufferPosition, pData, length );
    m_CrtBufferPosition += length;
    return length;
}

void SwitchBuffer::Flush(){
    std::lock_guard<std::mutex> al( m_LockCrtBuffer );
    if( m_CrtBufferPosition > 0 && m_BufferQueue.size() > 0 ){
        
        // 直接呼叫Notify函式，不再放到執行緒中等待呼叫，要不然會來不及寫入
        if( m_NotifyBufferFull != NULL )
            m_NotifyBufferFull( m_pCrtBuffer, m_CrtBufferPosition );
                
        m_BufferQueue.push( m_pCrtBuffer ); // return to queue for next time to be used        
            
        m_pCrtBuffer = m_BufferQueue.front();
        m_BufferQueue.pop();
        m_CrtBufferPosition = 0;
        assert( m_pCrtBuffer != NULL );
    }
}

void SwitchBuffer::DoSwitchBuffer(){    
    
    WriteBufferData wbData;
    wbData.pBuffer = m_pCrtBuffer;
    wbData.Length = m_CrtBufferPosition;
    m_NotifyQueue.Push( wbData );    
    
    m_pCrtBuffer = m_BufferQueue.front();
    m_BufferQueue.pop();
    m_CrtBufferPosition = 0;
    assert( m_pCrtBuffer != NULL );
    
}

void SwitchBuffer::NotifyThread(){
    while(true){
        try{
            WriteBufferData wbData = m_NotifyQueue.Pop();
            if( wbData.pBuffer != NULL && wbData.Length > 0 ){
                if( m_NotifyBufferFull != NULL )
                    m_NotifyBufferFull( wbData.pBuffer, wbData.Length );
                
                {
                    std::lock_guard<std::mutex> al( m_LockCrtBuffer );
                    m_BufferQueue.push( wbData.pBuffer ); // return to queue for next time to be used
                }
            }
        }catch(BQThreadAbort exp){            
            break;
        }catch(...){            
        }
    }
}

void SwitchBuffer::TimerNotifyThread(){
    while(true){
        if( m_TimerEvent.Wait(m_TimerPeriodMS) == false ){
            
            std::lock_guard<std::mutex> al( m_LockCrtBuffer );
            
            if( m_IsStop )
                break;
            
            // Timeout, switch buffer and notify 
            // 2015/03/10 Justin 如果BufferQueue中沒有可用的Buffer了，那此次Timeout就不通知
            if( m_CrtBufferPosition > 0 && m_BufferQueue.size() > 0 ){
                DoSwitchBuffer();
            }
        }else
            break;
    }
    
    m_TimerThreadStopped.Set(false);
}

void SwitchBuffer::SetTimerNotify( int periodMS, bool isEnable ){
    if( isEnable && !m_IsTimerRunning ){
        m_IsTimerRunning = isEnable;
        m_TimerPeriodMS = periodMS;
        std::thread tTimer( std::bind( &SwitchBuffer::TimerNotifyThread, this ) );
        tTimer.detach();
    }if( !isEnable && m_IsTimerRunning ){
        // stop
        m_TimerEvent.Set(false);
        
//        // 等待處理最後的Buffer
//        m_TimerThreadStopped.Wait( 5000 );  // 2015/03/10 Justin 可能在呼叫Wait()前TimerNotifyThread()就已經呼叫過Set()了，這樣會等不到
    }
}

#endif

}