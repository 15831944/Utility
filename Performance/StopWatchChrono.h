/* 
 * File:   StopWatchChrono.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年11月11日, 上午 10:09
 */

#ifndef UTILITY_STOPWATCHCHRONO_H
#define	UTILITY_STOPWATCHCHRONO_H

#include <chrono>

namespace ProgLab{ namespace Utility{

/// \brief 使用STL裏通用的時間函式庫Chrono來計算執行時間
/// 目前是用std::chrono::high_resolution_clock型別
class StopWatchChrono{
public:
    StopWatchChrono();
    StopWatchChrono(const StopWatchChrono& orig ) = delete;
    ~StopWatchChrono();
    
    void Start(){
        m_Start = std::chrono::high_resolution_clock::now();
    }
    void Stop(){
        m_End = std::chrono::high_resolution_clock::now();
        m_Diff = m_End - m_Start;
    }
    
    unsigned long ElapsedMicroSecond() const{
        return std::chrono::duration_cast<std::chrono::microseconds>( m_Diff ).count();
    }
    
    unsigned long ElapsedNanoSecond() const{
        return std::chrono::duration_cast<std::chrono::nanoseconds>( m_Diff ).count();
    }
    
    static unsigned long GetEPOCHNanoSecond(){
        return std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() ).count();
    }
    
private:
    std::chrono::high_resolution_clock::time_point  m_Start,m_End;
    std::chrono::high_resolution_clock::duration    m_Diff;
};

}
#endif	/* UTILITY_STOPWATCHCHRONO_H */

