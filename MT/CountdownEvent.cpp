/* 
 * File:   CountdownEvent.cpp
 * Author: 0007989
 * 
 * Created on 2014年7月21日, 下午 1:37
 */

#include "CountdownEvent.h"

namespace ProgLab{ namespace Utility{

CountdownEvent::CountdownEvent(int initCount ):m_Count(initCount) {   
}

CountdownEvent::CountdownEvent(const CountdownEvent& orig) {
}

CountdownEvent::~CountdownEvent() {    
}

void CountdownEvent::Reset( int initCount ){
    std::unique_lock<std::mutex> al(m_Locker);
    m_Count = initCount;
}

void CountdownEvent::Set(bool isBrocast ){
    try{
        std::unique_lock<std::mutex> al(m_Locker);
        m_Count--;
        if( m_Count <= 0 ){
            if( isBrocast )
                m_CV.notify_all();
            else 
                m_CV.notify_one();
        }
    }
    catch(...){    
    }
}

bool CountdownEvent::Wait( int waitMS){
    try{
        std::unique_lock<std::mutex> al(m_Locker);
        while( m_Count > 0 ){
            if( waitMS > 0 ){
                auto now = std::chrono::system_clock::now();
#ifdef _OS_LINUX_
#if __cplusplus >= 201103L
                std::cv_status cvstatus = m_CV.wait_until( al, now + std::chrono::milliseconds(waitMS) );
                return cvstatus == std::cv_status::no_timeout;
#else
		return m_CV.wait_until(al, now + std::chrono::milliseconds(waitMS));
#endif
#else // _WIN32_
		return m_CV.wait_until(al, now + std::chrono::milliseconds(waitMS)) == std::_Cv_status::no_timeout;
#endif
                
            }else
                m_CV.wait( al );
        }
        return true;
        
    }
    catch(...)
    {
        return false;
    }
    
}

}