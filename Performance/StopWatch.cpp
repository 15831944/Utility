/* 
 * File:   StopWatch.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月30日, 上午 8:00
 */

#include "StopWatch.h"


#include <sys/types.h>
#include <stdlib.h>


namespace KGI_TW_Der_Utility{

StopWatch::StopWatch() {
    Reset();
}

StopWatch::StopWatch(const StopWatch& orig) {
}

StopWatch::~StopWatch() {
}

void StopWatch::Reset(){
    m_StartTime = m_EndTime = m_StartCPUTime = m_EndCPUTime = 0;    
}
    
void StopWatch::Start(){
    GetTime( &m_StartTime, &m_StartCPUTime ); 
#ifdef _OS_LINUX_
    gettimeofday(&m_tStart, NULL);
#endif
}
    
void StopWatch::Stop(){
    GetTime( &m_EndTime, &m_EndCPUTime );
#ifdef _OS_LINUX_
    gettimeofday(&m_tEnd, NULL);
#endif
}
    
double StopWatch::ElapsedTime() const{
#ifdef _OS_LINUX_
    double diffSec = m_tEnd.tv_sec - m_tStart.tv_sec;
    double diffUs = m_tEnd.tv_usec - m_tStart.tv_usec;
    if( diffUs < 0.0 && diffSec > 0.0)
    {
        diffSec -= 1.0;
        diffUs = 1000000.0 + diffUs;
    }
    return diffSec * 1000.0 + diffUs / 1000.0;
#else
    return m_EndTime - m_StartTime;
#endif
}
    
double StopWatch::ElapsedCPUTime() const{
    return m_EndCPUTime - m_StartCPUTime;
}

#ifdef _OS_LINUX_
void StopWatch::GetTime(double* wcTime, double* cpuTime)
{
    struct timeval tp;

    gettimeofday(&tp, NULL);
    *wcTime=(double) (tp.tv_sec * 1000.0 + tp.tv_usec/1000.0);

    struct rusage ruse;
    getrusage(RUSAGE_SELF, &ruse);
    *cpuTime=(double)(ruse.ru_utime.tv_sec * 1000.0 +ruse.ru_utime.tv_usec / 1000.0);
}
#else
void StopWatch::GetTime(long* wcTime, long* cpuTime)
{
	*wcTime = PerformanceUtil::Instance().GetTick();
	*cpuTime = 0;
}
#endif




#ifdef _OS_LINUX_
void StopWatchEx::Reset(){
    m_Start.tv_nsec = m_Start.tv_sec = 0;
    m_End.tv_nsec = m_End.tv_sec = 0;
}

void StopWatchEx::Start(){
    clock_gettime(m_ClockID, &m_Start);
}
    
void StopWatchEx::Stop(){
    clock_gettime(m_ClockID, &m_End);
}
    
double StopWatchEx::ElapsedTime() const{
    int elapseSec = m_End.tv_sec - m_Start.tv_sec;
    int elapseNs = m_End.tv_nsec - m_Start.tv_nsec;
    if( elapseNs < 0 && elapseSec > 0 ){
        elapseSec--;
        elapseNs = 1000000000 + elapseNs;
    }
    return elapseSec * 1000.0 + elapseNs / 1000000.0;
}
#endif

}