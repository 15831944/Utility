/* 
 * File:   AtomicSeqNo.h
 * Author: 0007989
 *
 * Created on 2014年4月3日, 下午 5:42
 */

#ifndef UTILITY_ATOMICSEQNO_H
#define	UTILITY_ATOMICSEQNO_H

#include <CommonDef.h>
#include <mutex>

BEGIN_NAMESPACE(KGI_TW_Der_Utility)   

/*
 * 2014/08/11 Justin
 * Modified to use atomic operation function  https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
 */
class AtomicSeqNo {
public:
    AtomicSeqNo();
    AtomicSeqNo(const AtomicSeqNo& orig);
    virtual ~AtomicSeqNo();
    
    void Reset();
    long GetSeqNo() const;
    void SetSeqNo(long seqNo );
    long Increment();
    long operator++();
private:
#ifdef _OS_LINUX_
    std::mutex m_Locker;        
#endif
    long m_SeqNo;
};


END_NAMESPACE(KGI_TW_Der_Utility)

#endif	/* UTILITY_ATOMICSEQNO_H */

