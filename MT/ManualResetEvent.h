/* 
 * File:   ManualResetEvent.h
 * Author: 0007989
 *
 * Created on 2014年1月13日, 下午 3:25
 */

#ifndef UTILITY_MANUALRESETEVENT_H
#define	UTILITY_MANUALRESETEVENT_H

#include <mutex>
#include <condition_variable>

namespace KGI_TW_Der_Utility
{

/// ManualResetEven
///
class ManualResetEvent {
public:
    ManualResetEvent();
    virtual ~ManualResetEvent();
    
    void Reset();
    /// \param isBrocast 是否要通知所有等待的執行緒
    void Set(bool isBrocast );
    /// \param waitMS 要等待的時間(miliseconds)，0表示一直等待
    /// \return false if timeout
    bool Wait( int waitMS = 0);    
private:
    bool m_IsSignaled;
    std::mutex m_Lock;
    std::condition_variable m_CV;
};

}

#endif	/* UTILITY_MANUALRESETEVENT_H */

