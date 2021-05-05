/* 
 * File:   POSIXManualResetEvent.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年3月11日, 上午 9:51
 */

#ifndef UTILITY_POSIXMANUALRESETEVENT_H
#define	UTILITY_POSIXMANUALRESETEVENT_H

#ifdef _OS_LINUX_

#include <pthread.h>

namespace ProgLab{ namespace Utility{

/// \brief 使用POSIX的方式實作的MaunalResetEvent
/// 這種方式比較沒有可移植性，在windows系統下無法使用    
class POSIXManualResetEvent {
public:
    POSIXManualResetEvent();
    virtual ~POSIXManualResetEvent();
    
    void Reset();
    /// \param isBrocast 是否要通知所有等待的執行緒
    void Set(bool isBrocast );
    /// \param waitMS 要等待的時間(miliseconds)，0表示一直等待
    /// \return false if timeout
    bool Wait( int waitMS = 0);
private:
    pthread_mutex_t m_MutexEvent;
    pthread_cond_t m_MutexCondEvent;

};

#endif

}

#endif	/* UTILITY_POSIXMANUALRESETEVENT_H */

