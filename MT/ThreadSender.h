/* 
 * File:   ThreadSender.h
 * Author: 0007989
 *
 * Created on 2014年9月12日, 下午 2:31
 */

#ifndef UTILITY_THREADSENDER_H
#define	UTILITY_THREADSENDER_H

#include <thread>
#include "../Collect/BlockingQueue.h"

namespace KGI_TW_Der_Utility{

/// \brief use number of threads to send data to one receiver
template<typename T>
class ThreadSender{
public:
    ThreadSender(){}    
    
    void SetReceiver( std::function<void(T)> r ){
        m_Receiver = r;
    }
    
    /// \param c the thread count, default is 1
    void Start(int c = 1){
        m_Queue.Reset();
        while(c > 0 ){
            std::thread tThread( &ThreadSender::SendImpl, this );
            tThread.detach();
            c--;
        }
    }
    void Stop(){
        m_Queue.Abort();
    }
    
    void PushData( const T& data ){
        m_Queue.Push( data );
    }
private:
    void SendImpl();
private:
    BlockingQueue<T> m_Queue;
    std::function<void(T)> m_Receiver;
};

template<typename T>
void ThreadSender<T>::SendImpl(){
    try{
        while(true){
            T data = m_Queue.Pop();
            // dispatch
            m_Receiver(data);
        }
    }catch( BQThreadAbort exp ){
        // normal abort
        printf( "ThreadSender<T>::SendImpl() BlockingQueue abort thread\r\n" );
    }catch(...){
        
    }
}

}

#endif	/* UTILITY_THREADSENDER_H */

