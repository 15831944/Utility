/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MemoryPool.cpp
 * Author: Justin Lin
 * 
 * Created on 2017年11月17日, 下午 3:17
 */

#include <assert.h>

#include "MemoryPool.h"

namespace ProgLab{ namespace Utility{

MemoryPool::MemoryPool() {
}

MemoryPool::MemoryPool(const MemoryPool& orig) {
}

MemoryPool::~MemoryPool() {
}


/* ================= MemoryPoolFixSize =====================*/
MemoryPoolFixSize::MemoryPoolFixSize( int capacity, int objSize ):Capacity_(capacity),ObjSize_(objSize),pBuffer_(NULL),pHeader_(NULL){
    //ReAllocateCount_.store( 0 );
    pBuffer_ = new char[ Capacity_ * ObjSize_ ];

    for( int i = 0; i < Capacity_ - 1; ++i ){
        reinterpret_cast<SlotData*>( pBuffer_ + i * ObjSize_ )->NextSlot = reinterpret_cast<SlotData*>(pBuffer_ + (i + 1) * ObjSize_);        
    }
    reinterpret_cast<SlotData*>( pBuffer_ + (Capacity_-1) * ObjSize_ )->NextSlot = NULL;

    SlotData* ptmp = pHeader_.load();
    pHeader_.compare_exchange_weak( ptmp, reinterpret_cast<SlotData*>( pBuffer_ ) ); // 因 store( <ptr> )未實作，只好用CAS

    //assert( std::atomic<SlotData*>().is_lock_free() == true );
}

void* MemoryPoolFixSize::Allocate(){        
    SlotData* pAllocated = pHeader_.load();           
    while( !pHeader_.compare_exchange_weak( pAllocated, pAllocated->NextSlot) ){}
    if( pAllocated != NULL )
        return pAllocated;
    else
        return NULL;        
}
void MemoryPoolFixSize::Delete( void* pMemory ){  
    reinterpret_cast<SlotData*>( pMemory )->NextSlot = pHeader_.load();
    while( !pHeader_.compare_exchange_weak( reinterpret_cast<SlotData*>( pMemory )->NextSlot, reinterpret_cast<SlotData*>( pMemory )) ){}
}

}