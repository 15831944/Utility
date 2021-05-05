/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ScheduleNotifier.h
 * Author: Justin Lin
 *
 * Created on 2017年5月17日, 下午 12:59
 */

#ifndef UTILITY_SCHEDULENOTIFIER_H
#define UTILITY_SCHEDULENOTIFIER_H

#include <chrono>
#include <functional>
#include <thread>

#include "DateTime.h"
#include "MT/ManualResetEvent.h"
#include "SimpleTime.h"

namespace ProgLab{ namespace Utility{

template<typename TData>
class ScheduleNotifier {
public:
    typedef std::function<KGI_TW_Der_Utility::SimpleTime(const KGI_TW_Der_Utility::SimpleTime&, TData&)> GetNextScheduleFunc;
    typedef std::function<void(const TData&)>  ReceiveNotifyFunc;
    
    ScheduleNotifier(GetNextScheduleFunc func, ReceiveNotifyFunc recvFunc ):GetNextSchedule_(func), ReceiveNotify_(recvFunc){}
    ScheduleNotifier(const ScheduleNotifier& orig){}
    virtual ~ScheduleNotifier(){}
    
    void Start(){
        std::thread tScheduleThread( std::bind(&ScheduleNotifier::ScheduleThread,this) );
        tScheduleThread.detach();
    }
    void Stop(){
        EventStop_.Set(true);
    }
    
private:
    GetNextScheduleFunc GetNextSchedule_;
    ReceiveNotifyFunc   ReceiveNotify_;
    KGI_TW_Der_Utility::ManualResetEvent    EventStop_;
    
private:
    void ScheduleThread(){
        TData data;
        while( true ){
            if( GetNextSchedule_ != NULL ){
                DateTime now;
                SimpleTime stNow( now.GetHour(), now.GetMinute(), now.GetSecond() );                
                SimpleTime next = GetNextSchedule_(stNow, data);
                if( next.IsValid() ){
                    auto tpStart = next.to_time_point();
                    auto tpNow = stNow.to_time_point();            

                    auto waitTime = std::chrono::duration_cast<std::chrono::milliseconds>(tpStart - tpNow);
                    if( waitTime.count() > 0 ){
                        if( EventStop_.Wait( waitTime.count() ) == false ){
                            if( ReceiveNotify_ != NULL ){
                                ReceiveNotify_( data );
                            }
                        }else{
                            break; // stopped by external trigger
                        }
                    }
                }else 
                    break; // no more jobs
            }
        }
    }

};

}

#endif /* UTILITY_SCHEDULENOTIFIER_H */

