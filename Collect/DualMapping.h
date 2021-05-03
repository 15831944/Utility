/* 
 * File:   DualMapping.h
 * Author: 0007989
 *
 * Created on 2014年9月24日, 上午 11:00
 */

#ifndef UTILITY_DUALMAPPING_H
#define	UTILITY_DUALMAPPING_H

#include <map>
#include <list>

namespace KGI_TW_Der_Utility {

template<typename TMain, typename TSlave>
class DualMapping{
public:
    bool Add( TMain main, TSlave slave ){
        std::lock_guard<std::mutex> al(m_Lock);
        if( m_Map.find( main ) != m_Map.end() )
            return false;
        
        m_Map.insert( std::make_pair(main,slave) );
        typename std::map<TSlave,std::list<TMain>>::iterator itrList = m_SlaveMap.find( slave );
        if( itrList == m_SlaveMap.end() ){
            m_SlaveMap.insert( std::make_pair(slave,std::list<TMain>() ) );         
        }
            
        m_SlaveMap[slave].push_back( main );
            
        return true;
    }
    
    bool Remove( TMain main ){
        std::lock_guard<std::mutex> al(m_Lock);
        auto itrMain = m_Map.find( main );
        if( itrMain == m_Map.end() )
            return false;
        
        auto itrSlave = m_SlaveMap.find( itrMain->second );
        if( itrSlave != m_SlaveMap.end() ){
            itrSlave->second.remove( main );
            
            if( itrSlave->second.size() <= 0 )
                m_SlaveMap.erase( itrSlave );
        }
        
        m_Map.erase( itrMain );
        return true;        
    }
    bool Query( TSlave slave, std::list<TMain>& result ) const {
        std::lock_guard<std::mutex> al(m_Lock);
        
        auto itrSlave = m_SlaveMap.find( slave );
        if( itrSlave == m_SlaveMap.end() )
            return false;
        
        result.assign( itrSlave->second.begin(), itrSlave->second.end() );
        return true;
    }
    bool Query( TMain main, TSlave& result ) const{
        std::lock_guard<std::mutex> al(m_Lock);
        
        auto itrMain = m_Map.find( main );
        if( itrMain == m_Map.end() )
            return false;
        
        m_Map.erase( itrMain );
        return true;
    }
    int Size(){
        std::lock_guard<std::mutex> al(m_Lock);
        return m_Map.size();
    }
private:
    mutable std::mutex m_Lock;
    std::map<TMain,TSlave> m_Map;
    std::map<TSlave, std::list<TMain> > m_SlaveMap;
};

}

#endif	/* UTILITY_DUALMAPPING_H */

