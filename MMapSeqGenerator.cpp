/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MMapSeqGenerator.cpp
 * Author: Justin Lin
 * 
 * Created on 2018年6月8日, 下午 12:43
 */

#if defined(_OS_LINUX_)
// for Memory map file
#include <fcntl.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <unistd.h>
// end
#else // _WIN32
	// TODO
#endif 
#include <stdexcept>

#include "FileUtil.h"
#include "MMapSeqGenerator.h"

namespace ProgLab{ namespace Utility{

MMapSeqGenerator::MMapSeqGenerator( const std::string& file, unsigned long initValue ) : FileID_(-1){

    bool isExist = FileUtil::IsFileExist( file );

    FileID_ = open( file.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR );
    if( FileID_ < 0 ){
        throw std::runtime_error( "open file failed" );
    }else{
        FileSize_ = sizeof(std::atomic_ulong);
        if( !isExist ){
            // must seek to the last and write some thing to make the file does contain required size
            if( lseek( FileID_, FileSize_, SEEK_SET) == -1 ){
                close(FileID_);
                FileID_ = -1;

                throw std::runtime_error( "stretch file failed" );            
            }else{
                write( FileID_, " ", 1);            
                lseek( FileID_, 0, SEEK_SET); 
            }
        }
        
        pMMapFile_ = reinterpret_cast<char*>( mmap( 0,FileSize_,PROT_READ | PROT_WRITE, MAP_SHARED, FileID_, 0 ) ); // must be MAP_SHARED for write
        if( pMMapFile_ == MAP_FAILED ){
            close(FileID_);
            FileID_ = -1;
            
            throw std::runtime_error("create mmap failed" );
        }
        
        unsigned long orgValue = *(reinterpret_cast<unsigned long*>( pMMapFile_ ));                
        
        pSeqNumber_ = new(reinterpret_cast<void*>( pMMapFile_)) std::atomic_ulong(orgValue);
        if( !isExist ){
            pSeqNumber_->store( initValue );
        }
    }
}

MMapSeqGenerator::~MMapSeqGenerator(){
    if( pMMapFile_ != MAP_FAILED ){
        int totalSize = sizeof(std::atomic_ulong);
        munmap( pMMapFile_, totalSize );
        
        pMMapFile_ = reinterpret_cast<char*>( MAP_FAILED );
    }
    
    if( FileID_ > 0 ){
        close( FileID_ );
    }
    FileID_ = 0;
}

unsigned long MMapSeqGenerator::Increase(){
    return ++(*pSeqNumber_);
}

unsigned long MMapSeqGenerator::GetSequence() const{
    return pSeqNumber_->load();
}

unsigned long MMapSeqGenerator::operator++(){
    return ++(*pSeqNumber_);    
}

unsigned long MMapSeqGenerator::operator++(int){
    unsigned long tmp = pSeqNumber_->load();
    operator++();
    return tmp;
}


}
