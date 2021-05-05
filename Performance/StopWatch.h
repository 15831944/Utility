/* 
 * File:   StopWatch.h
 * Author: 0007989
 *
 * Created on 2014年4月30日, 上午 8:00
 */

#ifndef UTILITY_STOPWATCH_H
#define	UTILITY_STOPWATCH_H

#include "../MacroRecipe.h"

#ifdef _OS_LINUX_
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#else // _WIN32
#include "PerformanceUtil.h"
#endif

namespace ProgLab{ namespace Utility{

/// StopWath performance measuring tool
///
/// Record the system and cpu elapsed time at the same time.   
///
class StopWatch {
public:
    StopWatch();
    StopWatch(const StopWatch& orig);
    virtual ~StopWatch();
    
    void Reset();
    void Start();
    void Stop();
    
    /// \brief Get the elapsed time in ms
    /// \return elapsed time in ms
    double ElapsedTime() const;
    /// \brief Get the elapsed CPU time in ms
    /// \return elapsed time in ms
    double ElapsedCPUTime() const;
#ifdef _OS_LINUX_
    static void GetTime(double* wcTime, double* cpuTime);
#else
	static void GetTime(long* wcTime, long* cpuTime);
#endif
private:
#ifdef _OS_LINUX_
    double m_StartTime,m_EndTime;
    double m_StartCPUTime, m_EndCPUTime;
    
    
    struct timeval m_tStart, m_tEnd;
#else // _WIN32
	long m_StartTime, m_EndTime;
	long m_StartCPUTime, m_EndCPUTime;
#endif

};

#ifdef _OS_LINUX_
class StopWatchEx{
public:
    StopWatchEx( clockid_t clockID = CLOCK_REALTIME):m_ClockID(clockID){ Reset(); };
    virtual ~StopWatchEx(){};
    
    void Reset();
    void Start();
    void Stop();
    
    double ElapsedTime() const;
private:
    clockid_t m_ClockID;
    struct timespec m_Start, m_End;
};
#else // _WIN32
class StopWatchEx : public StopWatch{
public:
	StopWatchEx(int id){};
};
#endif 

}
#endif	/* UTILITY_STOPWATCH_H */

