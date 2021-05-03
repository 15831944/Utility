/* 
 * File:   DateTime.cpp
 * Author: 0007989
 * 
 * Created on 2014年1月28日, 上午 10:28
 */

#include "DateTime.h"
#include <cstddef>
#include <string.h>
#include <stdexcept>

namespace KGI_TW_Der_Utility
{   
    
char DateTime::m_MonthName[][4] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec","" };

DateTime::DateTime() {
    
    
#ifdef _OS_LINUX_
	gettimeofday(&m_TimeVal, 0);
	time_t curtime = m_TimeVal.tv_sec;
    localtime_r(&curtime, &m_TimeStructure); // for thread-safe use localtime_r()
#else
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	time_t curtime;
	time(&curtime);
	localtime_s(&m_TimeStructure, &curtime ); // for thread-safe use localtime_r()
	m_TimeVal.tv_sec = curtime;
	m_TimeVal.tv_usec = sysTime.wMilliseconds * 1000;
#endif
    //printf("%02d:%02d:%02d:%03d\n", t->tm_hour, t->tm_min, t->tm_sec, tp.tv_usec/1000);
}

DateTime::DateTime( const DateTime &src, int ms, int ns){
    *this = src;
    
    m_TimeVal.tv_usec =  ms * 1000 + ns;
}

DateTime::DateTime( const DateTime& src ){
    m_TimeVal = src.m_TimeVal;
    m_TimeStructure = src.m_TimeStructure;
}

DateTime::DateTime( int year, int month, int day, int hour, int minute, int second, int ms, int ns ){
    memset( &m_TimeStructure, 0, sizeof(m_TimeStructure) );
    m_TimeStructure.tm_year = year;
    m_TimeStructure.tm_mon = month;
    m_TimeStructure.tm_mday = day;
    m_TimeStructure.tm_hour = hour;
    m_TimeStructure.tm_min = minute;
    m_TimeStructure.tm_sec = second;
    
    m_TimeVal.tv_sec = mktime(&m_TimeStructure);
    if( m_TimeVal.tv_sec  == (time_t)-1 ){
        // Exception
        throw std::runtime_error( "DateTime connstruction failed due to invalide values");
    }
    
    m_TimeVal.tv_usec =  ms * 1000 + ns;
}

DateTime::~DateTime() {    
}

DateTime &DateTime::operator=(const DateTime &src ){
    m_TimeVal.tv_sec = src.m_TimeVal.tv_sec;
    m_TimeVal.tv_usec = src.m_TimeVal.tv_usec;
    
    time_t curtime = m_TimeVal.tv_sec;
#ifdef _OS_LINUX_
    localtime_r(&curtime, &m_TimeStructure); // for thread-safe use localtime_r()
#else
	localtime_s(&m_TimeStructure, &curtime ); // for thread-safe use localtime_r()
#endif

	return *this;
}

DateTime DateTime::Now(){
    DateTime tmp;
    return tmp;
}

void DateTime::AddDays( int dayDiff ){
    struct timeval diff;
    int absDayDiff = dayDiff > 0 ? dayDiff : -dayDiff;
    diff.tv_sec = 24 * 60 * 60 * absDayDiff;
    diff.tv_usec = 0;
    
    struct timeval result;
    if( dayDiff > 0 )
    {
        timeradd( &m_TimeVal, &diff, &result );        
    }
    else
    {
        timersub( &m_TimeVal, &diff, &result );
    }
    
    m_TimeVal = result;
    time_t curtime = m_TimeVal.tv_sec;
#ifdef _OS_LINUX_
	localtime_r(&curtime,&m_TimeStructure);
#else
	localtime_s(&m_TimeStructure, &curtime );
#endif    
    
}

#ifdef _OS_LINUX_
bool DateTime::Parse( const char *pTime, const char *pFmt ){ 
    char *pNotProcessed = strptime( pTime, pFmt, &m_TimeStructure );
    if( pNotProcessed == NULL )
        return false;
    
    time_t totalSec = mktime( &m_TimeStructure );
    m_TimeVal.tv_sec = totalSec;
    m_TimeVal.tv_usec = 0;    
    
    return true;
}
#endif

int DateTime::GetYear() const{
    return m_TimeStructure.tm_year + 1900;        
}

int DateTime::GetMonth() const{
    return m_TimeStructure.tm_mon + 1;        
}

int DateTime::GetDay() const{
    return m_TimeStructure.tm_mday;        
}

int DateTime::GetHour() const{
    return m_TimeStructure.tm_hour;        
}

int DateTime::GetMinute() const{
    return m_TimeStructure.tm_min;        
}

int DateTime::GetSecond() const{
    return m_TimeStructure.tm_sec;        
}

int DateTime::GetMillisecond() const{
    return m_TimeVal.tv_usec/1000;
}

int DateTime::GetMicrosecond() const{
    return m_TimeVal.tv_usec % 1000;
}

const char *DateTime::GetMonthString() const{
    return m_MonthName[m_TimeStructure.tm_mon+1];
}

time_t DateTime::GetEPOCHTime() const {
    return m_TimeVal.tv_sec * 1000000 + m_TimeVal.tv_usec;
}

time_t DateTime::GetEPOCHTimeEx(){
    //http://pubs.opengroup.org/onlinepubs/007904875/basedefs/xbd_chap04.html#tag_04_14
    // year > 1970
//    return m_TimeStructure.tm_sec + m_TimeStructure.tm_min*60 + m_TimeStructure.tm_hour*3600 + m_TimeStructure.tm_yday*86400 +
//        (m_TimeStructure.tm_year-70)*31536000 + ((m_TimeStructure.tm_year-69)/4)*86400 -
//        ((m_TimeStructure.tm_year-1)/100)*86400 + ((m_TimeStructure.tm_year+299)/400)*86400;        
    
    timeval timeVal; 
    gettimeofday(&timeVal, 0);
    return timeVal.tv_sec * 1000000 + timeVal.tv_usec;
}

bool DateTime::operator<( const DateTime& rhs ) const{
    if( GetYear() == rhs.GetYear() )
    {
        if( GetMonth() == rhs.GetMonth() )
        {
            if( GetDay() == rhs.GetDay() )
            {
                if( GetHour() == rhs.GetHour() )
                {
                    if( GetMinute() == rhs.GetMinute() )
                    {
                        if( GetSecond() == rhs.GetSecond() )
                        {
                            if( this->GetMillisecond() == rhs.GetMillisecond() )
                            {
                                return this->GetMicrosecond() < rhs.GetMicrosecond();
                            }
                            else
                                return this->GetMillisecond() < rhs.GetMillisecond();
                        }
                        else
                            return GetSecond() < rhs.GetSecond();
                    }
                    else
                        return GetMinute() < rhs.GetMinute();
                }
                else
                    return GetHour() < rhs.GetHour();
            }
            else
                return GetDay() < rhs.GetDay();
        }
        else
            return GetMonth() < rhs.GetMonth();
    }
    else
        return GetYear() < rhs.GetYear();    
}

bool DateTime::operator>( const DateTime& rhs ) const{
    return !((*this) < rhs);
}

bool DateTime::operator==( const DateTime& rhs ) const{
    return !((*this) > rhs) && !((*this) < rhs);
}

double DateTime::operator-( const DateTime& rhs ) const{
    double myMs = m_TimeVal.tv_sec * 1000.0 + m_TimeVal.tv_usec / 1000.0;
    double rhsMs = rhs.m_TimeVal.tv_sec * 1000.0 + rhs.m_TimeVal.tv_usec / 1000.0;
    return myMs - rhsMs;
}
#ifdef _LP64
unsigned long DateTime::GetTimestamp() const{
    return m_TimeVal.tv_sec * 1000000 + m_TimeVal.tv_usec;
}
#else
unsigned long long DateTime::GetTimestamp() const{
    return m_TimeVal.tv_sec * 1000000 + m_TimeVal.tv_usec;
}
#endif

}