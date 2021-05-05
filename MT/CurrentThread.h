/* 
 * File:   CurrentThread.h
 * Author: 0007989
 *
 * Created on 2015年1月19日, 下午 1:50
 */

#ifndef UTILITY_CURRENTTHREAD_H
#define	UTILITY_CURRENTTHREAD_H

#include <sys/types.h>
#include <thread>

#ifdef _OS_LINUX_

#if defined(_CYGWIN_)
#define THREAD_ID_TYPE std::thread::id
#define THREAD_LOCAL_VARIABLE __thread
#else
#define THREAD_ID_TYPE pid_t
#define THREAD_LOCAL_VARIABLE __thread
#endif

#else
#define THREAD_ID_TYPE DWORD
#define THREAD_LOCAL_VARIABLE __declspec(thread)

#include <Windows.h>
#endif



namespace ProgLab{ namespace Utility{ 

namespace CurrentThread{	
#if !defined(_CYGWIN_)
	extern THREAD_LOCAL_VARIABLE THREAD_ID_TYPE t_ThreadID;
#endif
	extern THREAD_LOCAL_VARIABLE const char* t_pThreadName;
	THREAD_ID_TYPE GetThreadID();
        std::string GetThreadIDString();
}

}
#endif	/* UTILITY_CURRENTTHREAD_H */

