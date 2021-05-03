/* 
 * File:   ObsoletedDataManager.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年7月14日, 上午 8:56
 */

#ifndef UTILITY_OBSOLETEDDATAMANAGER_H
#define	UTILITY_OBSOLETEDDATAMANAGER_H

#include <set>
#include <memory>
#include <list>
#include <chrono>
#include "../DateTime.h"

namespace KGI_TW_Der_Utility {    
    
template <typename TData>
class ObsoletedDataManager {
public:
    struct ObsoleteDataInfo{
        time_t TimeStamp;       //!< 標記要移除的時間點
        TData UID;              //!< data
        bool operator<( const ObsoleteDataInfo& rhs ) const{
            return TimeStamp < rhs.TimeStamp;
        }
    };
    typedef std::list<ObsoletedDataManager<TData>::ObsoleteDataInfo> ResultType;
    
    void Add( TData dataID ){
        std::lock_guard<std::mutex> al( m_LockSet );
        m_ObsoleteData.insert( ObsoleteDataInfo() = { DateTime::GetEPOCHTimeEx(), dataID } );
    }
    
    /// \brief 查詢在指定時間前放入的資料，並在查詢後由清單中移除
    /// \param timeLimit 指定在多久以前的資料要被查詢出來。需到microsecond的單位
    /// example:
    /// std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    /// std::time_t now_c = std::chrono::system_clock::to_time_t(now - std::chrono::seconds(5));
    /// std::list<ObsoletedDataManager<unsigned long>::ObsoleteDataInfo> result;
    /// Query( now_c * 1000000, result );
    void Query( time_t timeLimit, ResultType& result ){
        std::lock_guard<std::mutex> al( m_LockSet );
                
        auto itrLast = m_ObsoleteData.lower_bound( ObsoleteDataInfo() = { timeLimit, TData() } );
        result.assign( m_ObsoleteData.begin(), itrLast );   
        m_ObsoleteData.erase( m_ObsoleteData.begin(), itrLast ); 
    }
    
    int Size() const {
        std::lock_guard<std::mutex> al( m_LockSet );
        
        return m_ObsoleteData.size();
    }
private:
    mutable std::mutex m_LockSet;
    std::set<ObsoleteDataInfo> m_ObsoleteData;
};  

}

#endif	/* UTILITY_OBSOLETEDDATAMANAGER_H */

