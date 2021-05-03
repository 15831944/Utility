/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IntervalTrigger.cpp
 * Author: Justin Lin
 * 
 * Created on 2018年8月30日, 下午 6:31
 */

#include <unistd.h>

#include <thread>

#include "IntervalTrigger.h"

namespace KGI_TW_Der_Utility {

IntervalTrigger::IntervalTrigger(std::chrono::milliseconds delayMs,int threadCount) : DelayMs_(delayMs), ThreadCount_(threadCount)  {
    
}

//IntervalTrigger::IntervalTrigger(const IntervalTrigger& orig) {
//}

//IntervalTrigger::~IntervalTrigger() {
//}

void IntervalTrigger::Start(){
    
    EventNotifyStop_.clear();    
    
    for( int i = 0; i < ThreadCount_; ++i ){
        
        auto stopEvent = std::make_shared<ManualResetEvent>();
        EventNotifyStop_.push_back( stopEvent );
        stopEvent->Reset();
        
        std::thread tExecution( &IntervalTrigger::ExecuteJob, this, stopEvent  );
        tExecution.detach();
    }
}
    
void IntervalTrigger::Stop(){
    EventStop_.Reset( ThreadCount_ );
    
    Jobs_.Abort();
    
    for( auto itrEvent = EventNotifyStop_.begin(); itrEvent != EventNotifyStop_.end(); ++itrEvent ){
        (*itrEvent)->Set( true );
    }
    
    EventStop_.Wait( 5000 );
}    

void IntervalTrigger::Regist( std::function<bool(void)> func ){
    Jobs_.Push( Job{ std::chrono::system_clock::now(), func } );
}

void IntervalTrigger::ExecuteJob(std::shared_ptr<KGI_TW_Der_Utility::ManualResetEvent> event){
    while(true){
        try{
            auto job = Jobs_.Pop();
            
            auto crtTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( job.InTime + DelayMs_ - crtTime );
            if( duration.count() > 0 ){                
                if( event->Wait( duration.count() ) == true ){
                    // 外部通知要結束了
                    
                    break;
                }
            }            
            if( job.JobFunction() == false ){
                Regist( job.JobFunction );
            }
        }catch(...){
            break;
        }
    }
    EventStop_.Set( true );    
}

}

