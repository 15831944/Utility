/* 
 * File:   LockMappingData.h
 * Author: 0007989
 *
 * Created on 2014年8月26日, 下午 5:45
 */

#ifndef UTILITY_LOCKMAPPINGDATA_H
#define	UTILITY_LOCKMAPPINGDATA_H

#include <map>
#include <mutex>

namespace KGI_TW_Der_Utility {
    
template<typename TKey, typename TData>
class LockMappingData{
public:
    LockMappingData(){}
    virtual ~LockMappingData(){}
    
    void Add( TKey k, const TData& v );
    bool Query( TKey k, TData& result ) const;
    void Remove( TKey k );
    
protected:
    std::map<TKey, TData> m_mapData;
    mutable std::mutex m_lockData;
};  

template<typename TKey, typename TData>
void LockMappingData<TKey,TData>::Add( TKey k, const TData& v ){    
    std::unique_lock<std::mutex> al( m_lockData );
    
    m_mapData[k] = v;
}

template<typename TKey, typename TData>
bool LockMappingData<TKey,TData>::Query( TKey k, TData& result ) const{    
    std::unique_lock<std::mutex> al( m_lockData );
    
    typename std::map<TKey, TData>::const_iterator itrFound = m_mapData.find( k );
    if( itrFound == m_mapData.end() )
        return false;
    else{
        result = itrFound->second; // copy and return
        return true;
    }
}

template<typename TKey, typename TData>
void LockMappingData<TKey,TData>::Remove( TKey k ){
    std::unique_lock<std::mutex> al( m_lockData );
    m_mapData.erase( k );
}

}

#endif	/* UTILITY_LOCKMAPPINGDATA_H */

