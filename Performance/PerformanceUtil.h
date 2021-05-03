/* 
 * File:   PerformanceUtil.h
 * Author: 0007989
 *
 * Created on 2013年12月26日, 上午 9:04
 */

#ifndef UTILITY_PERFORMANCEUTIL_H
#define	UTILITY_PERFORMANCEUTIL_H

#include <memory>

namespace KGI_TW_Der_Utility
{

class PerformanceUtil {
public:
    static PerformanceUtil &Instance();
    
    virtual ~PerformanceUtil();

    double GetMicroSecondPerTick(){ return m_Multiplierus; }    
    long GetTick();
private:
    PerformanceUtil();
    
private:
    static std::unique_ptr<PerformanceUtil> m_Instance;
    double m_Multiplierus;    
#ifdef _OS_LINUX_
    clockid_t m_ClockID;
#endif

};

}

#endif	/* UTILITY_PERFORMANCEUTIL_H */

