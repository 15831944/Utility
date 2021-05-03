/* 
 * File:   CurrentThread.cpp
 * Author: 0007989
 * 
 * Created on 2015年1月19日, 下午 1:50
 */

#include "CurrentThread.h"

#ifdef _OS_LINUX_
#if !defined(_CYGWIN_)
#include <sys/syscall.h>    //SYS_gettid
#endif
#include <unistd.h>
#else
#include <Windows.h>
#endif
#include <sstream>

namespace KGI_TW_Der_Utility{ 

namespace CurrentThread{
    THREAD_LOCAL_VARIABLE const char* t_pThreadName = "unknown";
    
//    std::thread::id GetThreadID(){
//        return std::this_thread::get_id();
//    }
//    std::string GetThreadIDString(){
//        std::stringstream ss;
//        ss << GetThreadID();
//        return ss.str();
//    }
    
#if defined(_CYGWIN_)	
        THREAD_ID_TYPE GetThreadID(){
            return std::this_thread::get_id();
        }
        std::string GetThreadIDString(){
            std::stringstream ss;
            ss << GetThreadID();
            return ss.str();
        }
#else
        THREAD_LOCAL_VARIABLE THREAD_ID_TYPE t_ThreadID = 0;
	THREAD_ID_TYPE GetThreadID(){             
            if ( t_ThreadID == 0 ){
#ifdef _OS_LINUX_
                t_ThreadID = static_cast<THREAD_ID_TYPE>(::syscall(SYS_gettid));
#else
                t_ThreadID = GetCurrentThreadId();
#endif
            }
            return t_ThreadID;
        }
        
#endif // _CYGWIN_
}

}
