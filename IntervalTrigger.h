/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IntervalTrigger.h
 * Author: Justin Lin
 *
 * Created on 2018年8月30日, 下午 6:31
 */

#ifndef UTILITY_INTERVALTRIGGER_H
#define UTILITY_INTERVALTRIGGER_H

#include <chrono>
#include <functional>
#include <list>
#include <memory>

#include "./Collect/BlockingQueue.h"
#include "./MT/CountdownEvent.h"
#include "./MT/ManualResetEvent.h"

namespace KGI_TW_Der_Utility {

/// \brief 定時執行工作的排程器
class IntervalTrigger{
    struct Job{
        std::chrono::time_point<std::chrono::system_clock>      InTime;
        std::function<bool(void)>                               JobFunction;
    };
public:
    IntervalTrigger( std::chrono::milliseconds delayMs, int threadCount = 5 );
    IntervalTrigger( const IntervalTrigger& orig ) = delete;
    ~IntervalTrigger() = default;
    
    void Start();
    void Stop();
    
    /// \brief 註冊要執行的工作，執行後需傳回一個bool值，false表示要再重新註冊，等待下一次的執行
    ///
    void Regist( std::function<bool(void)> func );
private:
    std::chrono::milliseconds       DelayMs_;
    int                             ThreadCount_;
    
    KGI_TW_Der_Utility::BlockingQueue<Job>          Jobs_;
    CountdownEvent                  EventStop_;
    std::list<std::shared_ptr<KGI_TW_Der_Utility::ManualResetEvent>> EventNotifyStop_;
private:
    void ExecuteJob( std::shared_ptr<KGI_TW_Der_Utility::ManualResetEvent> event );
};

}

#endif /* UTILITY_INTERVALTRIGGER_H */

