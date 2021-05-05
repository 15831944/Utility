/* 
 * File:   CountdownEvent.h
 * Author: 0007989
 *
 * Created on 2014年7月21日, 下午 1:37
 */

#ifndef UTILITY_COUNTDOWNEVENT_H
#define	UTILITY_COUNTDOWNEVENT_H

//#include <pthread.h>
#include <mutex>
#include "ManualResetEvent.h"

namespace ProgLab{ namespace Utility{

class CountdownEvent {
public:
    CountdownEvent( int initCount = 1 );
    CountdownEvent(const CountdownEvent& orig);
    virtual ~CountdownEvent();
    
    /// \brief 重設計數器
    /// \param initCount 要重設的計數器值
    void Reset( int initCount );
    /// \brief 減少一個計數器
    /// \param isBrocast 是否通知所有等候者
    void Set(bool isBrocast );
    /// \brief 等待計數器歸零
    /// \param waitMS 等候時間
    bool Wait( int waitMS = 0);
private:    
    int m_Count;
    mutable std::mutex m_Locker;
    //ManualResetEvent m_Event;
    std::condition_variable m_CV;
};

}
#endif	/* UTILITY_COUNTDOWNEVENT_H */

