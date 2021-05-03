/* 
 * File:   PerformanceUtil.cpp
 * Author: 0007989
 * 
 * Created on 2013年12月26日, 上午 9:04
 */

#include "PerformanceUtil.h"

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#ifdef _OS_LINUX_
#include <sys/times.h>
#include <unistd.h>
#else // _WIN32
#include <Windows.h>
#endif

namespace KGI_TW_Der_Utility
{
    
std::unique_ptr<PerformanceUtil> PerformanceUtil::m_Instance( new PerformanceUtil() );

PerformanceUtil::PerformanceUtil() {
    std::cout << "PerformanceUtil()" << std::endl;   
#ifdef _OS_LINUX_
    // the following code muse link with -lrt
    pid_t mypid = getpid();
    m_ClockID = 0;
    int rc = clock_getcpuclockid( mypid, &m_ClockID );
    if( rc != 0 )
    {
        throw "PerformanceUtil() init failed";
    }
    struct timespec clockTimeSpec;
    memset( &clockTimeSpec, 0, sizeof( clockTimeSpec ) );
    rc = clock_getres( m_ClockID, &clockTimeSpec );
    if( rc < 0 )
        throw "PerformanceUtil() init failed";
    
    m_Multiplierus = clockTimeSpec.tv_nsec / 1000.0;
#else // _WIN32
	LARGE_INTEGER freq;
	freq.QuadPart = 0;
	::QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	if (freq.QuadPart > 0){
		m_Multiplierus = 1000000.0 / freq.QuadPart;
	}
#endif
    
}

PerformanceUtil::~PerformanceUtil() {
    std::cout << "~PerformanceUtil()" << std::endl;
}

PerformanceUtil &PerformanceUtil::Instance()
{   
    return (*m_Instance.get());
}

long PerformanceUtil::GetTick(){    
#ifdef _OS_LINUX_
    struct timespec clockTimeSpec;
    memset( &clockTimeSpec, 0, sizeof( clockTimeSpec ) );
    //int rc = clock_gettime( CLOCK_THREAD_CPUTIME_ID , &clockTimeSpec );
    int rc = clock_gettime( CLOCK_REALTIME  , &clockTimeSpec );
    return clockTimeSpec.tv_sec * 1000000 + clockTimeSpec.tv_nsec;
#else
	long tick = 0;
	::QueryPerformanceCounter((LARGE_INTEGER*)&tick);
	return tick;
#endif
}


}
