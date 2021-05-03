/* 
 * File:   SwitchBuffer.h
 * Author: 0007989
 *
 * Created on 2014年10月23日, 下午 4:17
 */

#ifndef UTILITY_SWITCHBUFFER_H
#define	UTILITY_SWITCHBUFFER_H

#define _SWITCHBUFFER_EX_
#define _SWITCHBUFFER_EX_DEBUG_

#include <stdio.h>
#include <mutex>
#include <functional>
#include <queue>
#include "Collect/BlockingQueue.h"
#include "MT/ManualResetEvent.h"

namespace KGI_TW_Der_Utility{

#ifdef _SWITCHBUFFER_EX_
/// 使用多個Buffer接收資料，當Buffer滿了時，就會切換，並通知外部人員Buffer滿的事件，以便讓外部人員可處理已填滿的Buffer
class SwitchBuffer {
    class WriteBufferData{
    public:
        WriteBufferData( int length ):m_pBuffer(NULL),m_Length(length),m_Index(0){
            m_pBuffer = new char[m_Length];      
#ifdef _SWITCHBUFFER_EX_DEBUG_
            printf( "WriteBufferData() Buffer(%p)\r\n", m_pBuffer );
#endif
        }
        ~WriteBufferData(){
#ifdef _SWITCHBUFFER_EX_DEBUG_
            printf( "~WriteBufferData() Buffer(%p)\r\n", m_pBuffer );
#endif
            delete[] m_pBuffer;
            m_pBuffer = NULL;
        }
        int GetLength() const{ return m_Length; }
        int GetIndex() const { return m_Index; }
        void SetIndex( int index ){ m_Index = index; }
        char* GetBuffer(){ return m_pBuffer; }
    private:
        char* m_pBuffer;
        int m_Length;
        int m_Index;
    };
public:
#if (__cplusplus >= 201103L) || (_MSC_VER >= 1800) // 因為在VS2013版中，它的__cplusplus仍是199711L
    SwitchBuffer( int bufferSize = 1024*1024, int numOfBuffers = 4, std::function<void(const char*,int)> notifyFunc = nullptr );    
#else
    SwitchBuffer(int bufferSize = 1024 * 1024, int numOfBuffers = 4, std::function<void(const char*, int)> notifyFunc = NULL);
#endif
    virtual ~SwitchBuffer();
    
    int WriteBuffer( const char* pData, int length );    
    void SetNotifyBufferFull( std::function<void(const char*,int)> notifyFunc ){ m_NotifyBufferFull = notifyFunc; }
    void SetTimerNotify( int periodMS, bool isEnable );    
    /// \brief 將目前Buffer裏的內容送出給使用者端處理
    void Flush();
    
    //void Stop();
private:
    int m_BufferSize;           //!< the buffer size of each buffer
    int m_NumOfBuffers;         //!< the total number of buffers
    std::shared_ptr<WriteBufferData> m_CrtBuffer;    
    //volatile int m_CrtBufferPosition;    //!< current using buffer index
        
    std::queue<std::shared_ptr<WriteBufferData>> m_BufferQueue;                //!< all available buffers
    BlockingQueue<std::shared_ptr<WriteBufferData>> m_NotifyQueue;   //!< for notify receiver the buffer is ready for use
        
    mutable std::mutex m_LockCrtBuffer;
    
    bool m_IsTimerRunning;          //!< is timer need to running or not
    ManualResetEvent m_TimerEvent;
    ManualResetEvent m_TimerThreadStopped;
    int m_TimerPeriodMS;            //!< time period for notify receiver, even the buffer is not full
    
    bool m_IsStopTimerNotify;                  //!< stop switch buffer
    
    std::function<void(const char*,int)> m_NotifyBufferFull;        // callback function for notifying the current buffer is full
private:
    void DoSwitchBuffer();
    void NotifyThread();
    void TimerNotifyThread();
};    
#else
/// 使用多個Buffer接收資料，當Buffer滿了時，就會切換，並通知外部人員Buffer滿的事件，以便讓外部人員可處理已填滿的Buffer
class SwitchBuffer {
    struct WriteBufferData{
        char* pBuffer;
        int Length;
    };
public:
#if (__cplusplus >= 201103L) || (_MSC_VER >= 1800) // 因為在VS2013版中，它的__cplusplus仍是199711L
    SwitchBuffer( int bufferSize = 1024*1024, int numOfBuffers = 2, std::function<void(const char*,int)> notifyFunc = nullptr );    
#else
    SwitchBuffer(int bufferSize = 1024 * 1024, int numOfBuffers = 2, std::function<void(const char*, int)> notifyFunc = NULL);
#endif
    virtual ~SwitchBuffer();
    
    int WriteBuffer( const char* pData, int length );    
    void SetNotifyBufferFull( std::function<void(const char*,int)> notifyFunc ){ m_NotifyBufferFull = notifyFunc; }
    void SetTimerNotify( int periodMS, bool isEnable );    
    /// \brief 將目前Buffer裏的內容送出給使用者端處理
    void Flush();
    
    //void Stop();
private:
    int m_BufferSize;           //!< the buffer size of each buffer
    int m_NumOfBuffers;         //!< the total number of buffers
    char* m_pCrtBuffer;         //!< current using buffer
    volatile int m_CrtBufferPosition;    //!< current using buffer index
        
    std::queue<char*> m_BufferQueue;                //!< all available buffers
    BlockingQueue<WriteBufferData> m_NotifyQueue;   //!< for notify receiver the buffer is ready for use
        
    mutable std::mutex m_LockCrtBuffer;
    
    bool m_IsTimerRunning;          //!< is timer need to running or not
    ManualResetEvent m_TimerEvent;
    ManualResetEvent m_TimerThreadStopped;
    int m_TimerPeriodMS;            //!< time period for notify receiver, even the buffer is not full
    
    bool m_IsStop;                  //!< stop switch buffer
    
    std::function<void(const char*,int)> m_NotifyBufferFull;        // callback function for notifying the current buffer is full
private:
    void DoSwitchBuffer();
    void NotifyThread();
    void TimerNotifyThread();
};
#endif

}
#endif	/* UTILITY_SWITCHBUFFER_H */

