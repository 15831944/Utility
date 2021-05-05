/* 
 * File:   POSIXManualResetEvent.cpp
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 * 
 * Created on 2015年3月11日, 上午 9:51
 */

#ifdef _OS_LINUX_

#include "POSIXManualResetEvent.h"

#include <sys/time.h>

namespace ProgLab{ namespace Utility{

POSIXManualResetEvent::POSIXManualResetEvent() {
    pthread_mutex_init( &m_MutexEvent, NULL );
    pthread_cond_init( &m_MutexCondEvent, NULL );    
}

POSIXManualResetEvent::~POSIXManualResetEvent() {
    pthread_mutex_destroy(&m_MutexEvent);
    pthread_cond_destroy(&m_MutexCondEvent);
}

void POSIXManualResetEvent::Reset(){  
}

void POSIXManualResetEvent::Set(bool isBrocast){   
    if( isBrocast)
        pthread_cond_broadcast(&m_MutexCondEvent);
    else
        pthread_cond_signal(&m_MutexCondEvent );
}

bool POSIXManualResetEvent::Wait( int waitMS){
    if( waitMS > 0 )
    {
        struct timeval now;
        ::gettimeofday( &now, NULL);
        struct timespec waitTime;
        waitTime.tv_nsec = now.tv_usec;
        waitTime.tv_sec = now.tv_sec + waitMS / 1000;
        int rc = pthread_cond_timedwait( &m_MutexCondEvent, &m_MutexEvent, &waitTime);
        return rc == 0;
    }
    else
    {   
        int rc = pthread_cond_wait(&m_MutexCondEvent, &m_MutexEvent);
        return rc == 0;
    }
}

}
#endif

