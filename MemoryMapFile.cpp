/* 
 * File:   MemoryMapFile.cpp
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 * 
 * Created on 2016年2月4日, 上午 11:28
 */

#include "MemoryMapFile.h"
#include <string.h>

#if defined(_OS_LINUX_)
#include <sys/types.h>
#include <unistd.h>
#endif

namespace KGI_TW_Der_Utility{ 

#ifdef _OS_LINUX_

MemoryMapFile::MemoryMapFile(const std::string& filename, int filesize, int flags, mode_t mode) 
    : m_FileID(-1), m_FileSize( filesize ),m_pMMapFile(reinterpret_cast<char*>(MAP_FAILED)){
    m_FileID = open( filename.c_str(), flags, mode );
    if( m_FileID < 0 ){
        throw std::runtime_error( "open file failed" );
    }else{
        // must seek to the last and write some thing to make the file does contain required size
        if( lseek( m_FileID, m_FileSize-1, SEEK_SET) == -1 ){
            close(m_FileID);
            m_FileID = -1;
            
            throw std::runtime_error( "stretch file failed" );            
        }else{
            write( m_FileID, " ", 1);
            
            //lseek( fid, 0, SEEK_SET); // need not to seek to the begining
        }
        
        m_pMMapFile = reinterpret_cast<char*>( mmap( 0,m_FileSize,PROT_READ | PROT_WRITE, MAP_SHARED, m_FileID, 0 ) ); // must be MAP_SHARED for write
        if( m_pMMapFile == MAP_FAILED ){
            close(m_FileID);
            m_FileID = -1;
            
            throw std::runtime_error("create mmap failed" );
        }
    }
}

//MemoryMapFile::MemoryMapFile(const MemoryMapFile& orig) {
//}

MemoryMapFile::~MemoryMapFile() {
    if( m_pMMapFile != MAP_FAILED ){
        munmap( m_pMMapFile, m_FileSize );
        m_pMMapFile = reinterpret_cast<char*>( MAP_FAILED );
    }
    
    close( m_FileID );
}

int MemoryMapFile::Write( const char* pData, int length, int offset){
    if( m_FileID < 0 || m_pMMapFile == MAP_FAILED ){
        return 0;
    }
    {
        std::lock_guard<std::mutex> al( m_LockFile );
        if( offset + length >= m_FileSize ){
            return 0;
        }else{        
            memcpy( m_pMMapFile + offset, pData, length );
            return length;
        }
    }
}

int MemoryMapFile::Read( char* pBuffer, int length, int offset){
    if( m_FileID < 0 || m_pMMapFile == MAP_FAILED ){
        return 0;
    }
    {
        std::lock_guard<std::mutex> al( m_LockFile );
        if( offset + length >= m_FileSize ){
            return 0;
        }else{
            memcpy( pBuffer, m_pMMapFile + offset, length );
            return length;
        }
    }
}

#else
#endif

}

