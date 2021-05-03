/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SpinLock.h
 * Author: Justin Lin
 *
 * Created on 2016年10月24日, 下午 6:21
 */

#ifndef UTILITY_SPINLOCK_H
#define UTILITY_SPINLOCK_H

#if defined(_OS_LINUX_)
#include "../CommonDef.h"
#if __cplusplus >= 201103L
#include <atomic>
#else
#include <cstdatomic>
#endif
#else	// _WIN32
#include "../CommonDef.h"
#if _MSVC_LANG  >= 201103L
#include <atomic>
#else
#include <cstdatomic>
#endif

#endif

BEGIN_NAMESPACE(KGI_TW_Der_Utility)

class SpinLocker{
public:
    SpinLocker() /*: m_Lock( ATOMIC_FLAG_INIT )*/{ m_Lock.clear(); }
    
    void lock(){
        while( m_Lock.test_and_set() );
    }
    
    void unlock(){
        m_Lock.clear();
    }
private:
    std::atomic_flag m_Lock;
};

END_NAMESPACE(KGI_TW_Der_Utility)
#endif /* UTILITY_SPINLOCK_H */

