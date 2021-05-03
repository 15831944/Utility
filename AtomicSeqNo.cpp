/* 
 * File:   AtomicSeqNo.cpp
 * Author: 0007989
 * 
 * Created on 2014年4月3日, 下午 5:42
 */

#include "AtomicSeqNo.h"

namespace KGI_TW_Der_Utility{  

AtomicSeqNo::AtomicSeqNo() {
    Reset();
}

AtomicSeqNo::AtomicSeqNo(const AtomicSeqNo& orig) {
}

AtomicSeqNo::~AtomicSeqNo() {
}

void AtomicSeqNo::Reset(){
    m_SeqNo = 0;
}

long AtomicSeqNo::GetSeqNo() const
{
    return m_SeqNo;
}

void AtomicSeqNo::SetSeqNo(long seqNo ){
#if defined(_OS_WIN_)
    m_SeqNo = seqNo;
#elif defined(_OS_LINUX_)
    while(true){
        long org = m_SeqNo;
        if( __sync_bool_compare_and_swap( &m_SeqNo, org, seqNo ) )
            break;
        //printf( "fail\r\n" );
    }
#endif
}

long AtomicSeqNo::operator++(){
#if defined(_OS_WIN_)
	long result = InterlockedIncrementAcquire( &m_SeqNo );
	return result;        
#elif defined(_OS_LINUX_)
        long result = __sync_add_and_fetch( &m_SeqNo, 1 );
        return result;
#else
        m_SeqNo++;
        return m_SeqNo;
#endif       
}

long AtomicSeqNo::Increment()
{
#if defined(_OS_WIN_)
	long result = InterlockedIncrementAcquire( &m_SeqNo );
	return result;        
#elif defined(_OS_LINUX_)
        long result = __sync_add_and_fetch( &m_SeqNo, 1 );
        return result;
#else
        m_SeqNo++;
        return m_SeqNo;
#endif        
}

}

