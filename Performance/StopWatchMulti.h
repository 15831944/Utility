/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StopWatchMulti.h
 * Author: Justin Lin
 *
 * Created on 2016年11月10日, 上午 8:16
 */

#ifndef UTILITY_STOPWATCHMULTI_H
#define UTILITY_STOPWATCHMULTI_H

#include <chrono>

namespace KGI_TW_Der_Utility{

/// \brief 支援量測多個點的StopWatch工具
/// 可用於當一個函式中要再細分成多個段落，並計算每個量測點跟起點或任兩點間的時間
class StopWatchMulti {
    typedef std::chrono::high_resolution_clock::time_point TimePointType;
public:
    explicit StopWatchMulti( int count );
    StopWatchMulti(const StopWatchMulti& orig) = delete;
    ~StopWatchMulti();
    
    /// \brief 開始量測，記錄起始點的時間
    void Start(){
        if( m_pTimePoints ){
            m_pTimePoints[0] = std::chrono::high_resolution_clock::now();
        }
    }
    
    /// \brief 將目前的時間標記到指定的索引位置中
    /// \param 要標記的索引位置
    void Mark( int idx ){
        if( idx > 0 && idx < m_Total && m_pTimePoints != NULL ){
            m_pTimePoints[idx] = std::chrono::high_resolution_clock::now();
        }
    }
    
    /// \brief 傳回某一點跟起始點的時間差，單位為ns
    /// \param idx 要量測的點
    /// \return 某一點跟起始點的時間差，單位為ns
    unsigned long ElapsedNanoSecond( int idx ) const{
        if( idx > 0 && idx < m_Total && m_pTimePoints != NULL ){
            return std::chrono::duration_cast<std::chrono::nanoseconds>( m_pTimePoints[idx] - m_pTimePoints[0] ).count();        
        }else
            return 0;            
    }
    
    /// \brief 傳回任兩點間的時間差，單位為ns
    /// \param idxStart 起始點
    /// \param idxEnd   終點
    /// \return 兩點間的時間差，單位為ns
    unsigned long ElapsedNanoSecond( int idxStart, int idxEnd ) const{
        if( idxStart >= 0 && idxStart < m_Total && m_pTimePoints != NULL && idxEnd >= 0 && idxEnd < m_Total ){
            return std::chrono::duration_cast<std::chrono::nanoseconds>( m_pTimePoints[idxEnd] - m_pTimePoints[idxStart] ).count();        
        }else
            return 0;            
    }
private:
    int             m_Total;
    TimePointType*  m_pTimePoints;
};

}
#endif /* UTILITY_STOPWATCHMULTI_H */

