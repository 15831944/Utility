/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SimpleTime.cpp
 * Author: Justin Lin
 * 
 * Created on 2017年4月28日, 上午 9:16
 */

#include "SimpleTime.h"

#include "StringTemplateUtil.h"

namespace KGI_TW_Der_Utility{  
       
SimpleTime::SimpleTime(int hour, int minute, int second):m_Hour(hour),m_Minute(minute),m_Second(second){    
}

SimpleTime::SimpleTime( const SimpleTime& orig ){
    *this = orig;
}

SimpleTime& SimpleTime::operator=( const SimpleTime& orig ){
    if( this != &orig ){
        m_Hour = orig.m_Hour;
        m_Minute = orig.m_Minute;
        m_Second = orig.m_Second;
    }
    return *this;
}

bool SimpleTime::operator<( const SimpleTime& rhs ) const{
    if( m_Hour == rhs.m_Hour ){
        if( m_Minute == rhs.m_Minute ){
            return m_Second < rhs.m_Second;
        }else
            return m_Minute < rhs.m_Minute;
    }else
        return m_Hour < rhs.m_Hour;
}

bool SimpleTime::operator==( const SimpleTime& rhs ) const{
    return m_Hour == rhs.m_Hour && m_Minute == rhs.m_Minute && m_Second == rhs.m_Second;
}

std::string SimpleTime::ToString() const{
    std::string result;
    StringTemplateUtil::Format( result, "%02d:%02d:%02d", m_Hour, m_Minute, m_Second );
    return result;
}

SimpleTime& SimpleTime::operator+=( const SimpleTime& rhs ){
    int s = m_Second + rhs.m_Second;
    m_Second = s % 60;
    int m = m_Minute + rhs.m_Minute + s / 60;
    m_Minute = m % 60;
    int h = m_Hour + rhs.m_Hour + m / 60;
    m_Hour = h % 24;
    return *this;
}

SimpleTime& SimpleTime::operator-=( const SimpleTime& rhs ){
    SimpleTime tmp( *this );
    
    if( tmp < rhs ){
        // 如果 *this 比 rhs 少的話，就先反過來相減後再處理
        SimpleTime t(rhs);
        t -= tmp;
        m_Hour = 0 - t.m_Hour;
        m_Minute = 0 - t.m_Minute;
        m_Second = 0 - t.m_Second;
        return *this;
    }else{
        int borrowHour = 0, borrowMinute = 0;
        if( tmp.m_Second < rhs.m_Second ){
            borrowMinute = 1;
            tmp.m_Second = tmp.m_Second + ( 60 - rhs.m_Second );
        }else
            tmp.m_Second -= rhs.m_Second;
        tmp.m_Minute -= borrowMinute;
        if( tmp.m_Minute < rhs.m_Minute ){
            borrowHour = 1;
            tmp.m_Minute = tmp.m_Minute + ( 60 - rhs.m_Minute );
        }else
            tmp.m_Minute -= rhs.m_Minute;
        tmp.m_Hour -= borrowHour;
        tmp.m_Hour -= rhs.m_Hour;

        *this = tmp;
        return *this;
    }
}

SimpleTime& SimpleTime::AddMinute( int minute ){
    if( minute <= 0 )
        return *this;

    int m = m_Minute + minute;
    m_Minute = m % 60;
    int h = m_Hour + m / 60;
    m_Hour = h % 24;
    return *this;
}

}
