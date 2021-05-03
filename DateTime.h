/* 
 * File:   DateTime.h
 * Author: 0007989
 *
 * Created on 2014年1月28日, 上午 10:28
 */

#ifndef UTILITY_DATETIME_H
#define	UTILITY_DATETIME_H

#include <time.h>
#ifdef _OS_LINUX_
#include <sys/time.h>
#else
#include <WinSock2.h>
#endif

namespace KGI_TW_Der_Utility
{   

class DateTime {
public:
    DateTime();    
    DateTime( const DateTime& src );
    DateTime( const DateTime &src, int ms, int ns );
    /// \param year 西元年 - 1900
    /// \param month 要先減 1
    DateTime( int year, int month, int day, int hour, int minute, int second, int ms = 0, int ns = 0 );
    virtual ~DateTime();
    
    DateTime &operator=(const DateTime &src );
    
    static DateTime Now();
    
    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;
    int GetHour() const;
    int GetMinute() const;
    int GetSecond() const;
    int GetMillisecond() const;
    int GetMicrosecond() const;
    // the epoch time is in microsecond
    time_t GetEPOCHTime() const;
    
    static time_t GetEPOCHTimeEx();
    
#ifdef _LP64
    /// \brief Get timestamp with epoch time in nanosecond
    /// \return timestamp with epoch time in nanosecond
    unsigned long GetTimestamp() const;
#else
    /// \brief Get timestamp with epoch time in nanosecond
    /// \return timestamp with epoch time in nanosecond
    unsigned long long GetTimestamp() const;
#endif
    
    const char *GetMonthString() const;
    
    void AddDays( int dayDiff );
    
    bool operator>( const DateTime& rhs ) const;
    bool operator<( const DateTime& rhs ) const;
    bool operator==( const DateTime& rhs ) const;
    /// \breif calculate the time difference in ms
    double operator-( const DateTime& rhs ) const;
#ifdef _OS_LINUX_
    ///
    /// \param pFmt ref:http://linux.die.net/man/3/strptime
    /// \return when failed return false
    bool Parse( const char *pTime, const char *pFmt );
#endif
    
private:
    timeval m_TimeVal;    
    struct tm m_TimeStructure;
    static char m_MonthName[][4];

#ifndef _OS_LINUX_
private:
	void timeradd(struct timeval* pLHS, struct timeval* pRHS, struct timeval* pResult){
		long tmp = pLHS->tv_usec + pRHS->tv_usec;
		pResult->tv_usec = tmp % 1000000;
		pResult->tv_sec = pLHS->tv_sec + pRHS->tv_sec + tmp / 1000000;
	}

	void timersub(struct timeval* pLHS, struct timeval* pRHS, struct timeval* pResult){
		long tmp = pLHS->tv_sec - pRHS->tv_sec;
		pResult->tv_usec = pLHS->tv_usec - pRHS->tv_usec;
		if (pResult->tv_usec < 0 && pResult->tv_sec > 0){
			pResult->tv_sec--;
			pResult->tv_usec += 1000000;
		}		
	}
#endif

};

}

#endif	/* UTILITY_DATETIME_H */

