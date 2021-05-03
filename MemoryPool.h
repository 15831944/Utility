/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MemoryPool.h
 * Author: Justin Lin
 *
 * Created on 2017年11月17日, 下午 3:17
 */

#ifndef UTILITY_MEMORYPOOL_H
#define UTILITY_MEMORYPOOL_H

#include "CommonDef.h"

#if defined(_OS_LINUX_)
#if __cplusplus >= 201103L
#include <atomic>
#else
#include <cstdatomic>
#endif
#else	// _WIN32
#if _MSVC_LANG  >= 201103L
#include <atomic>
#else
#include <cstdatomic>
#endif
#endif

#include <memory>

namespace KGI_TW_Der_Utility{

class MemoryPool {
public:
    MemoryPool();
    MemoryPool(const MemoryPool& orig);
    virtual ~MemoryPool();
private:

};

/// \brief 固定配置指定數目的記憶體區塊的 MemoryPool
class MemoryPoolFixSize{
public:
    MemoryPoolFixSize( int capacity, int objSize );
    MemoryPoolFixSize( const MemoryPoolFixSize& orig ) = delete;
    virtual ~MemoryPoolFixSize(){}
    
    MemoryPoolFixSize& operator=( const MemoryPoolFixSize& rhs ) = delete; 

    void* Allocate();
    void Delete( void* pMemory );

    //unsigned int GetReAllocateCount() const  { return ReAllocateCount_.load(); }    
private:
    /// \brief 內部使用資料型別
    union SlotData{
        char*   pBuffer;
        SlotData*   NextSlot;
    };    
private:    
    int Capacity_;                                  //!< 總共配置數目
    int ObjSize_;                                   //!< 每個記憶體區塊的大小
    char* pBuffer_;                                 //!< 配置出來的記憶體
    std::atomic<SlotData*> pHeader_;                //!< 目前可用的記憶體
    //std::atomic<unsigned int> ReAllocateCount_;     //!< 配置失敗時重新配置的計數器
};


template< typename T >
class MemoryPoolFixSizeAllocator : public MemoryPoolFixSize{
public:
    MemoryPoolFixSizeAllocator( int capacity ) : MemoryPoolFixSize( capacity, sizeof(T) ){}
    MemoryPoolFixSizeAllocator( const MemoryPoolFixSizeAllocator& orig ) = delete;
    virtual ~MemoryPoolFixSizeAllocator(){}
    
    template<typename ... Args>
    std::shared_ptr<T> MakeShared( std::weak_ptr<MemoryPoolFixSizeAllocator<T>> mp, Args ... params ){
        auto lockMP = mp.lock();
        if( lockMP ){            
            void* pRaw = lockMP->Allocate();
            if( pRaw != NULL ){
                T* ptrObj = new( pRaw ) T(params ...);
                return std::shared_ptr<T>( ptrObj, std::bind( MemoryPoolFixSizeDeleter, std::placeholders::_1, mp) );
            }else{
                // MemoryPool無法配置時，就改取用 OS 的 heap
                return std::make_shared<T>( params ... );
            }
        }
        else
            return std::shared_ptr<T>();
    }
    
    static void MemoryPoolFixSizeDeleter( void* pMem, std::weak_ptr<MemoryPoolFixSizeAllocator<T>> mp ){    
        auto lockMP = mp.lock();
        if( lockMP ){
            lockMP->Delete( pMem ); 
        }
    }    
};

/// \brief 提供給 char buffer 使用
template< int charSize >
class MemoryPoolFixSizeCharAllocator : public MemoryPoolFixSize{
public:
    MemoryPoolFixSizeCharAllocator( int capacity ) : MemoryPoolFixSize( capacity, charSize ){}
    MemoryPoolFixSizeCharAllocator( const MemoryPoolFixSizeCharAllocator& orig ) = delete;
    virtual ~MemoryPoolFixSizeCharAllocator(){}
        
    std::shared_ptr<char> MakeShared( std::weak_ptr<MemoryPoolFixSize> mp ){
        auto lockMP = mp.lock();
        if( lockMP ){            
            void* pRaw = lockMP->Allocate();
            if( pRaw != NULL ){                
                return std::shared_ptr<char>( pRaw, std::bind( MemoryPoolFixSizeDeleter, std::placeholders::_1, mp) );
            }else{
                // MemoryPool無法配置時，就改取用 OS 的 heap
                return std::make_shared<char>( new char[charSize] );
            }
        }
        else
            return std::shared_ptr<char>();
    }
    
    static void MemoryPoolFixSizeDeleter( void* pMem, std::weak_ptr<MemoryPoolFixSize> mp ){    
        auto lockMP = mp.lock();
        if( lockMP ){
            lockMP->Delete( pMem ); 
        }
    }    
};

}
#endif /* UTILITY_MEMORYPOOL_H */

