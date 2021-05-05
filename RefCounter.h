/* 
 * File:   RefCounter.h
 * Author: 0007989
 *
 * Created on 2014年10月29日, 下午 1:18
 */

#ifndef UTILITY_REFCOUNTER_H
#define	UTILITY_REFCOUNTER_H

#include <thread>
#include <functional>
#include <stdio.h>
#if !defined(_OS_WIN_) && !defined(_OS_LINUX_)
#include <mutex>
#endif

namespace ProgLab{ namespace Utility{  

class RefCounter {
public:
    RefCounter( int init = 0 ):m_Count(init){}
    ~RefCounter(){}
    
    int Add(){ 
#ifdef _OS_WIN_
		long result = InterlockedIncrementAcquire(&m_Count);
		return result;
#else 
#ifdef _OS_LINUX_
        return __sync_add_and_fetch(&m_Count,1); 
#else
		m_Count++;
		return m_Count;
#endif
#endif
    }
    
    int Release(){
#ifdef _OS_WIN_
		long result = InterlockedDecrementAcquire(&m_Count);
		return result;
#else 
#ifdef _OS_LINUX_
        return __sync_sub_and_fetch(&m_Count,1); 
#else
		m_Count--;
		return m_Count;
#endif
#endif
    }
    
    int Count(){ return m_Count; }
private:
#if !defined(_OS_WIN_) && !defined(_OS_LINUX_)
	mutable std::mutex m_Lock;
#endif
    volatile int m_Count;

};

}

#endif	/* UTILITY_REFCOUNTER_H */

