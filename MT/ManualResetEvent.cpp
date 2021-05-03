/* 
 * File:   ManualResetEvent.cpp
 * Author: 0007989
 * 
 * Created on 2014年1月13日, 下午 3:25
 */

//#include <sys/time.h>

#include "ManualResetEvent.h"

namespace KGI_TW_Der_Utility
{

ManualResetEvent::ManualResetEvent():m_IsSignaled(false) {
}

ManualResetEvent::~ManualResetEvent() {
}

void ManualResetEvent::Reset(){  
    std::unique_lock<std::mutex> al( m_Lock );
    m_IsSignaled = false;
}

void ManualResetEvent::Set(bool isBrocast){   
    std::unique_lock<std::mutex> al( m_Lock );
    m_IsSignaled = true;
    if( isBrocast )
        //std::notify_all_at_thread_exit( m_CV, std::move(m_Lock) ); // missing function        
        m_CV.notify_all();
    else
        m_CV.notify_one();           
}

bool ManualResetEvent::Wait( int waitMS){

    std::unique_lock<std::mutex> al( m_Lock );
    while(!m_IsSignaled){
        if( waitMS > 0 ){
            auto now = std::chrono::system_clock::now();
            
#ifdef _OS_LINUX_
#if __cplusplus >= 201103L
        std::cv_status cvstatus = m_CV.wait_until( al, now + std::chrono::milliseconds(waitMS) );
        return cvstatus == std::cv_status::no_timeout;
#else
        return m_CV.wait_until( al, now + std::chrono::milliseconds(waitMS) );
#endif
#else // _WIN32_
        return m_CV.wait_until(al, now + std::chrono::milliseconds(waitMS)) == std::_Cv_status::no_timeout;
#endif
        }
        else
            m_CV.wait( al );
    }
    return true;
}

}
