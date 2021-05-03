/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MemoryAllocator.cpp
 * Author: Justin Lin
 * 
 * Created on 2017年3月14日, 上午 8:30
 */

#include "MemoryAllocator.h"

namespace KGI_TW_Der_Utility{

#if defined(_OS_LINUX_)
// for Memory map file
#include <fcntl.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
// end
#else // _WIN32
	// TODO
#endif 

#include <string.h>
#include <stdexcept>
#include <list>

HeapMemoryAllocator::HeapMemoryAllocator( int objSize, int objNum) : ObjSize_(objSize), ObjNum_(objNum) {
    Pool_ = malloc( (objSize + 1) * objNum ); // +1 是要多配置一個byte來進行管理
    memset( Pool_, 0, (objSize + 1) * objNum );
    for( int i = 0; i < objNum; ++i ){
        void* pChunk = Pool_ + (objSize + 1) * i;
        void* pRealObjAddress = pChunk + 1;
        AvailableQueue_.push( pRealObjAddress );
    }
}

//MemoryAllocator::MemoryAllocator(const MemoryAllocator& orig) {
//}

HeapMemoryAllocator::~HeapMemoryAllocator() {
    free( Pool_ );    
}

void* HeapMemoryAllocator::Allocate(){
    std::lock_guard<decltype(Lock_)> al( Lock_ );
    if( AvailableQueue_.empty() == false ){
        void* pRealObjAddress = AvailableQueue_.front();
        AvailableQueue_.pop();
        void* pChunk = pRealObjAddress - 1;
        char* pChunkMark = static_cast<char*>( pChunk );
        *pChunkMark = 1;
        return pRealObjAddress;
    }else
        return NULL;
}

void HeapMemoryAllocator::Release( void* pObjMemory ){
    std::lock_guard<decltype(Lock_)> al( Lock_ );
    void* pChunk = pObjMemory - 1;
    char* pChunkMark = static_cast<char*>( pChunk );
    *pChunkMark = 0;
    AvailableQueue_.push( pObjMemory );
}

int HeapMemoryAllocator::GetAvailableCount() const{
    std::lock_guard<decltype(Lock_)> al( Lock_ );
    return AvailableQueue_.size();
}

void HeapMemoryAllocator::GetUsedList( std::list<void*>& usedList ){
    usedList.clear(); // 因為是由heap配置來的，所以一開始都是新的，沒有已用過的
}

#if defined(_OS_LINUX_)
MMapMemoryAllocator::MMapMemoryAllocator(const std::string& filename,int objSize, int objNum, bool isReuse ) 
    : Filename_(filename),ObjSize_(objSize), ObjNum_(objNum), IsReuse_(isReuse) {
    
    bool isExist = IsFileExist( filename );
    
#ifdef _OS_LINUX_
    FileID_ = open( Filename_.c_str() , O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);    
#else // _WIN32
    FileID_ = ::_fsopen( Filename_.c_str() , "a+", _SH_DENYWR);
#endif
    
    if( FileID_ <= 0 ){
        throw std::runtime_error("Creat file failed");
    }
    
    int totalSize = (ObjSize_ + 1) * ObjNum_;
    // 調整檔案的大小，不然 memory map會失敗
    if( lseek( FileID_, totalSize-1, SEEK_SET) == -1 ){
        close(FileID_);
        FileID_ = 0;

        throw std::runtime_error( "Stretch file failed" );        
    }else{
        write( FileID_, " ", 1);
    }
    
    // must be MAP_SHARED for write
    MMapFile_ = reinterpret_cast<char*>( mmap( 0, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, FileID_, 0 ) ); 
    if( MMapFile_ == MAP_FAILED ){
        close(FileID_);
        FileID_ = 0;

        throw std::runtime_error("create mmap failed" );        
    }
    
    if( !isExist ) // 新檔案的話，才先清除內容
        memset( MMapFile_, 0, totalSize );
    
    for( int i = 0; i < ObjNum_; ++i ){
        char* pChunk = MMapFile_ + (ObjSize_ + 1) * i;
        char* pRealObjAddress = pChunk + 1;
        if( *pChunk == 1 ){
            // 如果剛載入時是先前已使用過的資料，就放到待回補清單中
            UsedList_.push_back( pRealObjAddress );
        }else{
            AvailableQueue_.push( pRealObjAddress );
        }
    }
}

MMapMemoryAllocator::~MMapMemoryAllocator(){
    if( MMapFile_ != MAP_FAILED ){
        int totalSize = (ObjSize_ + 1) * ObjNum_;
        munmap( MMapFile_, totalSize );
        
        MMapFile_ = reinterpret_cast<char*>( MAP_FAILED );
    }
    
    if( FileID_ > 0 ){
        close( FileID_ );
    }
    FileID_ = 0;
}

bool MMapMemoryAllocator::IsFileExist( const std::string& filename ){
    struct stat fileStat;
    int result = stat( filename.c_str(), & fileStat );
    if( result != 0 ){
        return false;
    }else{
        return fileStat.st_size > 0 ;
    }
}

void* MMapMemoryAllocator::Allocate(){
    std::lock_guard<decltype(Lock_)> al( Lock_ );
    if( AvailableQueue_.empty() == false ){
        char* pRealObjAddress = AvailableQueue_.front();
        AvailableQueue_.pop();
        char* pChunkMark = pRealObjAddress - 1;        
        *pChunkMark = 1;
        return pRealObjAddress;
    }else
        return NULL;
}

void MMapMemoryAllocator::Release( void* pObjMemory ){
    std::lock_guard<decltype(Lock_)> al( Lock_ );
    void* pChunk = pObjMemory - 1;
    char* pChunkMark = static_cast<char*>( pChunk );
    if( IsReuse_ ){
        *pChunkMark = 0;
        AvailableQueue_.push( static_cast<char*>(pObjMemory) );
    }
}

int MMapMemoryAllocator::GetAvailableCount() const{
    std::lock_guard<decltype(Lock_)> al( Lock_ );
    return AvailableQueue_.size();
}

void MMapMemoryAllocator::GetUsedList( std::list<void*>& usedList ){
    for( auto itr = UsedList_.begin(); itr != UsedList_.end(); ++itr ){
        usedList.push_back( *itr );
    }
}
#else // _WIN32
// TODO
#endif

}