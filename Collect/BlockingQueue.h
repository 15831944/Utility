/* 
 * File:   BlockingQueue.h
 * Author: 0007989
 *
 * Created on 2014年8月18日, 上午 8:13
 */

#ifndef UTILITY_BLOCKINGQUEUE_H
#define	UTILITY_BLOCKINGQUEUE_H

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <functional>

namespace KGI_TW_Der_Utility {
    
class BQThreadAbort : public std::exception{    
};

template <typename TData, typename TLock = std::mutex >
class BlockingQueue{
public:
    BlockingQueue() : m_IsAbort(false){}
    virtual ~BlockingQueue(){}
        
    void Push( TData data ){
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);
        m_Queue.push( data );
        m_Condition.notify_one();
    }    
    TData Pop(){
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);
        while( m_Queue.empty() && !m_IsAbort ){
            m_Condition.wait( lock );
        }
        if( m_IsAbort )
            throw BQThreadAbort();
        else{
            TData result = m_Queue.front();
            m_Queue.pop();

            return result;
        }
    }
    int Size() const{
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);
        return m_Queue.size();
    }
    /// \brief 透過指定的接收函式，清空目前 Queue 中的所有資料
    /// \param flushReceiver 資料接收函式
    void Flush( std::function<void(TData)> flushReceiver ){
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);
        while( m_Queue.size() >0 ){
            flushReceiver( m_Queue.front() );
            m_Queue.pop();
        }    
    }
    
    /// \brief 重設，並清除所有資料
    void Reset(){
        m_IsAbort = false;
    
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);    
        std::queue<TData> empty;
        std::swap( m_Queue, empty );
    }
    void Abort(){
        m_IsAbort = true;
        m_Condition.notify_all(); // 2015/03/10 Justin 需要改為用notify_all()。當有多個執行緒在等待這個訊息時，使用notify_all()才能通知到所有的執行緒    
    }
private:
    std::queue<TData> m_Queue;
    mutable TLock m_Lock;
    std::condition_variable m_Condition;
    bool m_IsAbort;
};

/// \brief 支援 move 功能
///
template <typename TData, typename TLock = std::mutex >
class BlockingQueueEx{
public:
    BlockingQueueEx() : m_IsAbort(false){}
    virtual ~BlockingQueueEx(){}
    
    // 20170609 Justin 將型別改為 TData&& 以便接收可以 move 的資料
    void Push( TData&& data ){
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);
        m_Queue.push( std::move(data) );
        m_Condition.notify_one();
    }    
    TData Pop(){
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);
        while( m_Queue.empty() && !m_IsAbort ){
            m_Condition.wait( lock );
        }
        if( m_IsAbort )
            throw BQThreadAbort();
        else{
            TData result = std::move( m_Queue.front() );
            m_Queue.pop();

            return std::move( result );
        }
    }
    int Size() const{
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);
        return m_Queue.size();
    }
    /// \brief 透過指定的接收函式，清空目前 Queue 中的所有資料
    /// \param flushReceiver 資料接收函式
    void Flush( std::function<void(TData)> flushReceiver ){
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);
        while( m_Queue.size() >0 ){
            flushReceiver( m_Queue.front() );
            m_Queue.pop();
        }    
    }
    
    /// \brief 重設，並清除所有資料
    void Reset(){
        m_IsAbort = false;
    
        std::unique_lock<decltype(m_Lock)> lock(m_Lock);    
        std::queue<TData> empty;
        std::swap( m_Queue, empty );
    }
    void Abort(){
        m_IsAbort = true;
        m_Condition.notify_all(); // 2015/03/10 Justin 需要改為用notify_all()。當有多個執行緒在等待這個訊息時，使用notify_all()才能通知到所有的執行緒    
    }
private:
    std::queue<TData> m_Queue;
    mutable TLock m_Lock;
    std::condition_variable m_Condition;
    bool m_IsAbort;
};

}
#endif	/* UTILITY_BLOCKINGQUEUE_H */

