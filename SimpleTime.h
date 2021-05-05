/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SimpleTime.h
 * Author: Justin Lin
 *
 * Created on 2017年4月28日, 上午 9:16
 */

#ifndef UTILITY_SIMPLETIME_H
#define UTILITY_SIMPLETIME_H

#include <string>
#include <chrono>
#include <stdexcept>

#include "DateTime.h"

namespace ProgLab{ namespace Utility{  

/// \brief 簡單的時間處理類別
class SimpleTime{
public:
    //SimpleTime():m_Hour(0),m_Minute(0),m_Second(0){};
    /// \brief default constructor
    /// \param hour 時
    /// \param minute 分
    /// \param second 秒
    SimpleTime( int hour = 0, int minute = 0, int second = 0 );
    SimpleTime( const SimpleTime& orig );
    
    SimpleTime& operator=( const SimpleTime& orig );
    bool operator<( const SimpleTime& rhs ) const;    
    bool operator==( const SimpleTime& rhs ) const;
    
    /// \brief 判斷物件是否有效
    /// 判斷方式為只要 hour / minute / second 其中一個不為0時，即為有效值
    /// \return 是否有效
    bool IsValid() const{ 
        return m_Hour != 0 || m_Minute != 0 || m_Second != 0;
    }
    
    /// \brief 取得小時
    /// \return 小時
    int GetHour() const { return m_Hour; }
    /// \brief 取得分鐘
    /// \return 分鐘
    int GetMinute() const { return m_Minute; }
    /// \brief 取得秒數
    /// \return 秒數
    int GetSecond() const { return m_Second; }
    
    void SetHour( int hour ){ 
        if( hour < 0 || hour > 24 )
            throw std::runtime_error( "Invalid hour" );
        m_Hour = hour; 
    }
    void SetMinute( int minute ){ 
        if( minute < 0 || minute > 60 )
            throw std::runtime_error( "Invalid minute" );
        m_Minute = minute; 
    }
    void SetSecond( int second ){ 
        if( second < 0 || second > 60 )
            throw std::runtime_error( "Invalid second" );
        m_Second = second; 
    }
    
    SimpleTime& operator+=( const SimpleTime& rhs );    
    SimpleTime& operator-=( const SimpleTime& rhs );
    
    /// \brief 累加指定的分鐘
    /// \param 要累加的分鐘
    /// \return 累加後的物件本身參考
    SimpleTime& AddMinute( int minute );
    
    std::string ToString() const;
    
    // http://stackoverflow.com/questions/25393683/stdstring-to-stdchrono-time-point
    /// \brief 計算 time_point 以便跟std::chrono套件配合使用來計算時間差
    template <typename Clock_t = std::chrono::high_resolution_clock,
            typename MicroSecond_t = std::chrono::microseconds>
    typename Clock_t::time_point to_time_point(){
        
        using KGI_TW_Der_Utility::DateTime;
        
        DateTime now;
        
        std::tm simpleTimeTM;
        simpleTimeTM.tm_year = now.GetYear();
        simpleTimeTM.tm_mon = now.GetMonth() - 1;
        simpleTimeTM.tm_mday = now.GetDay();
        simpleTimeTM.tm_hour = m_Hour;
        simpleTimeTM.tm_min = m_Minute;
        simpleTimeTM.tm_sec = m_Second;
        
        auto time_c = mktime(&simpleTimeTM);
        return Clock_t::from_time_t(time_c) + MicroSecond_t{0};
    }
private:
    int m_Hour;             //!< 時
    int m_Minute;           //!< 分
    int m_Second;           //!< 秒
        
};

}

#endif /* UTILITY_SIMPLETIME_H */

