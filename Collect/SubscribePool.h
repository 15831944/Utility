/*
 * File:   SubscribePool.h
 * Author: 0007989
 *
 * Created on 2014年4月28日, 下午 3:52
 */

#ifndef UTILITY_SUBSCRIBEPOOL_H
#define	UTILITY_SUBSCRIBEPOOL_H

#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include <mutex>
#include <memory>

namespace KGI_TW_Der_Utility {

template<typename TKey>
struct QueryFunctor : std::binary_function<TKey,TKey,bool>{
        virtual bool operator()( const TKey& lhs, const TKey& rhs ) const = 0;
    };

template<typename T>
struct CompSharedPtrOpr{
public:
    bool operator()( const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs ){
        return ( !lhs.owner_before( rhs ) && !rhs.owner_before( lhs ) );
    }
};

template<typename T>
struct CompWeakPtrOpr{
public:
    bool operator()( const std::weak_ptr<T>& lhs, const std::weak_ptr<T>& rhs ){
        return ( !lhs.owner_before( rhs ) && !rhs.owner_before( lhs ) );
    }
};

template<typename TKey, typename TData>
class SubscribePool
{
public:

public:
    SubscribePool(){}
    ~SubscribePool(){}

    typedef std::map<TKey,std::list<TData>> PoolType;

    int Size() const{
        int totalSize = 0;
        std::unique_lock<std::mutex> al( m_LockPool );
        for( typename std::map<TKey,std::list<TData>>::const_iterator itrMap = m_SubList.begin(); itrMap != m_SubList.end(); ++itrMap ){
            totalSize += itrMap->second.size();
        }
        return totalSize;
    }

    void Clear(){
        m_SubList.clear();
    }

    bool Add( TKey key, TData data ){

        try
        {
            std::unique_lock<std::mutex> al( m_LockPool );

            typename PoolType::iterator itrFound = m_SubList.find( key );
            if( itrFound != m_SubList.end() )
            {
                itrFound->second.push_back( data );
            }
            else
            {
                std::list<TData> newList;
                newList.push_back( data );
                m_SubList.insert( std::pair<TKey,std::list<TData>> ( key, newList ) );
            }
        }
        catch( ... )
        {
            return false;
        }
        return true;
    }
    /// \return TRUE : if not exist and added
    bool AddIfNotExist( TKey key, TData data ){
        bool isAdded = false;

        try
        {
            std::unique_lock<std::mutex> al( m_LockPool );

            typename PoolType::iterator itrFound = m_SubList.find( key );
            if( itrFound != m_SubList.end() )
            {
                if( std::find( itrFound->second.begin(), itrFound->second.end(), data ) == itrFound->second.end() ){
                    itrFound->second.push_back(data);
                    isAdded = true;
                }
            }
            else
            {
                std::list<TData> newList;
                newList.push_back( data );
                m_SubList.insert( std::pair<TKey,std::list<TData>> ( key, newList ) );
                isAdded = true;
            }
        }
        catch( ... )
        {
            return false;
        }
        return isAdded;
    }
    /// \return TRUE : if not exist and added
    template<typename c> bool AddIfNotExist( TKey key, TData data, c equal ){
        bool isAdded = false;

        try
        {
            std::unique_lock<std::mutex> al( m_LockPool );

            typename PoolType::iterator itrFound = m_SubList.find( key );
            if( itrFound != m_SubList.end() )
            {
                for ( typename std::list<TData>::iterator itr = itrFound->second.begin(); itr != itrFound->second.end(); ++itr )
                {
                    if( equal( (*itr) ,data) )
                    {
                        return false;
                    }
                }
                itrFound->second.push_back(data);
                isAdded = true;
            }
            else
            {
                std::list<TData> newList;
                newList.push_back( data );
                m_SubList.insert( std::pair<TKey,std::list<TData>> ( key, newList ) );
                isAdded = true;
            }
        }
        catch( ... )
        {
            return false;
        }
        return isAdded;
    }
    bool Remove( TKey key, TData data ){
        bool result = false;

        try
        {
            std::unique_lock<std::mutex> al( m_LockPool );

            typename PoolType::iterator itrFound = m_SubList.find( key );
            if( itrFound != m_SubList.end() )
            {
                for( typename std::list<TData>::iterator itr = itrFound->second.begin(); itr != itrFound->second.end(); ++itr )
                {
                    if( (*itr) == data )
                    {
                        itrFound->second.erase( itr );
                        result = true;
                        break;
                    }
                }
            }
        }
        catch(...)
        {
            result = false;
        }
        return result;
    }
    template<typename c> bool Remove( TKey key, TData data, c equal  ){
        bool result = false;

        try
        {
            std::unique_lock<std::mutex> al( m_LockPool );

            typename PoolType::iterator itrFound = m_SubList.find( key );
            if( itrFound != m_SubList.end() )
            {
                for( typename std::list<TData>::iterator itr = itrFound->second.begin(); itr != itrFound->second.end(); ++itr )
                {
                    if( equal( (*itr) ,data) )
                    {
                        itrFound->second.erase( itr );
                        result = true;
                        break;
                    }
                }
            }
        }
        catch(...)
        {
            result = false;
        }
        return result;
    }
    bool RemoveAll( TData data ){
        bool result = true;

        try
        {
            std::unique_lock<std::mutex> al( m_LockPool );

            for( typename std::map<TKey,std::list<TData>>::iterator itrMap = m_SubList.begin(); itrMap != m_SubList.end(); ++itrMap )
            {
                for( typename std::list<TData>::iterator itr = itrMap->second.begin(); itr != itrMap->second.end(); ++itr )
                {
                    if( (*itr) == data )
                    {
                        // http://stackoverflow.com/questions/2874441/deleting-elements-from-stl-set-while-iterating
                        // when remove iterator in for loop, must move forward before erase() it
                        itrMap->second.erase( itr++ );
                    }
                }
            }

        }
        catch(...)
        {
            result = false;
        }
        return result;
    }

    template<typename c> bool RemoveAll( TData data, c comp ){
        bool result = true;

        try
        {
            std::unique_lock<std::mutex> al( m_LockPool );

            for( typename std::map<TKey,std::list<TData>>::iterator itrMap = m_SubList.begin(); itrMap != m_SubList.end(); ++itrMap )
            {
                for( typename std::list<TData>::iterator itr = itrMap->second.begin(); itr != itrMap->second.end(); ++itr )
                {
                    if( comp( (*itr) ,data) )
                    {
                        // http://stackoverflow.com/questions/2874441/deleting-elements-from-stl-set-while-iterating
                        // when remove iterator in for loop, must move forward before erase() it
                        itrMap->second.erase( itr++ );
                    }
                }
            }

        }
        catch(...)
        {
            result = false;
        }
        return result;
    }

    /// \brief Query TData with given key
    /// \param key an instance of TKey for searching
    /// \param result for receiving a copy of found TData list
    /// \return Success of Failed
    bool Query( const TKey &key, std::list<TData>& result ) const{
        // this function could not be 'const', because the lock is not constable

        try
        {
            std::unique_lock<std::mutex> al( m_LockPool );

            typename PoolType::const_iterator itrFound = m_SubList.find( key );
            if( itrFound != m_SubList.end() )
            {
                // Copy
                result.assign( itrFound->second.begin(), itrFound->second.end() );
            }

        }
        catch( ... )
        {
            return false;
        }
        return true;
    }

    /// \brief To use std::equal_to<TKey>, must declare a class which is derived from std::equal_to<TKey> and implement operator()
    /// \param key an instance of TKey for searching
    /// \param result for receiving a copy of found TData list
    //  \param funcCompare the functor for compare if two TKey is equal or not
    /// \return Success of Failed
//    bool Query( const TKey& key, std::list<TData>& result, QueryFunctor<TKey>& funcCompare ){
//        m_LockPool.Lock();
//        try
//        {
//            for( typename PoolType::iterator itrSearch = m_SubList.begin(); itrSearch != m_SubList.end(); ++itrSearch )
//            {
//                if( funcCompare((const TKey&)itrSearch->first, (const TKey&)key) == true )
//                {
//                    //result.merge( itrSearch->second ); // 2014/05/07 Justin, because mearge() needs to implement operator<(), modified to use insert()
//                    result.insert( result.end(), itrSearch->second.begin(), itrSearch->second.end() );
//                }
//            }
//
//            m_LockPool.UnLock();
//        }
//        catch( ... )
//        {
//            m_LockPool.UnLock();
//            return false;
//        }
//        return true;
//    }

    template<typename Pred>
    bool Query( const TKey& key, std::list<TData>& result, Pred funcCompare ) const{

        try
        {
            std::unique_lock<std::mutex> al( m_LockPool );
            for( typename PoolType::const_iterator itrSearch = m_SubList.begin(); itrSearch != m_SubList.end(); ++itrSearch )
            {
                //if( funcCompare((const TKey&)itrSearch->first, (const TKey&)key) == true )
                if( funcCompare(itrSearch->first, key) == true )
                {
                    //result.merge( itrSearch->second ); // 2014/05/07 Justin, because mearge() needs to implement operator<(), modified to use insert()
                    result.insert( result.end(), itrSearch->second.begin(), itrSearch->second.end() );
                }
            }
        }
        catch( ... )
        {
            return false;
        }
        return true;
    }

    typedef typename std::map<TKey,std::list<TData>>::iterator PoolIterator;
    typedef typename std::map<TKey,std::list<TData>>::const_iterator PoolConstIterator;

    PoolConstIterator begin() const{ return m_SubList.begin(); }
    PoolConstIterator end() const{ return m_SubList.end(); }

protected:
    typename std::map<TKey,std::list<TData>> m_SubList;
    mutable std::mutex m_LockPool;
};


}
#endif	/* UTILITY_SUBSCRIBEPOOL_H */

