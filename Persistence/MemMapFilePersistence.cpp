/* 
 * File:   MemMapFilePersistence.cpp
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 * 
 * Created on 2016年2月16日, 上午 11:32
 */

#include "MemMapFilePersistence.h"

#ifdef _OS_LINUX_
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <memory.h>

namespace KGI_TW_Der_Utility{ 

MemMapFilePersistence::MemMapFilePersistence(int fileSize) : m_FileID(0),m_FileSize(fileSize),m_pMMapFile(reinterpret_cast<char*>(MAP_FAILED)),m_OrgFileSize(0){
}

//MemMapFilePersistence::MemMapFilePersistence(const MemMapFilePersistence& orig) {
//}

MemMapFilePersistence::~MemMapFilePersistence() {
}

bool MemMapFilePersistence::Open( const char* pFilename ){
    if( m_FileSize <= 0 )
        return false;
    
    m_FilePos = 0;    
    
#ifdef _OS_LINUX_
    m_FileID = open( pFilename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);    
#else // _WIN32
    m_FileID = ::_fsopen( pFilename, "a+", _SH_DENYWR);
#endif   
    if( m_FileID < 0 ){
        //throw std::runtime_error( "open file failed" );
        return false;
    }else{
        // must seek to the last and write some thing to make the file does contain required size
        int orgFileSize = GetFileSize();
        m_FilePos = orgFileSize; // 將讀寫指標移到檔案尾端
        if( orgFileSize < m_FileSize ){
            if( lseek( m_FileID, m_FileSize-1, SEEK_SET) == -1 ){
                close(m_FileID);
                m_FileID = -1;

                //throw std::runtime_error( "stretch file failed" );
                return false;
            }else{
                write( m_FileID, " ", 1);

                //lseek( fid, 0, SEEK_SET); // need not to seek to the begining
            }
        }else
            m_FileSize = orgFileSize;
        
        m_pMMapFile = reinterpret_cast<char*>( mmap( 0,m_FileSize,PROT_READ | PROT_WRITE, MAP_SHARED, m_FileID, 0 ) ); // must be MAP_SHARED for write
        if( m_pMMapFile == MAP_FAILED ){
            close(m_FileID);
            m_FileID = -1;
            
            //throw std::runtime_error("create mmap failed" );
            return false;
        }
        
        return true;
    }
}

void MemMapFilePersistence::Close(){
    if( m_pMMapFile != MAP_FAILED ){
        munmap( m_pMMapFile, m_FileSize );
        m_pMMapFile = reinterpret_cast<char*>( MAP_FAILED );
    }
    
    if( m_FileID > 0 )
    {
        // 調整檔案的大小
        ::ftruncate( m_FileID, m_FilePos );
                
        ::close( m_FileID );
        m_FileID = 0;
    }
}

int MemMapFilePersistence::SeekFromHead( int offset ){
    if( IsValid() ){
        m_FilePos = offset;
        return m_FilePos;
    }else
        return -1;
}
    
int MemMapFilePersistence::SeedFromTail( int offset ){
    if( IsValid() ){
        m_FilePos = m_FileSize - offset;
        return m_FilePos;
    }else
        return -1;
}

int MemMapFilePersistence::Write( const char* pBuffer, int length ){
    
    int writeCount = 0;
    if( IsValid() ){
        std::lock_guard<std::mutex> al( m_LockFile );
        
        if( m_FilePos + length < m_FileSize ){
            writeCount = length;
            memcpy( m_pMMapFile + m_FilePos, pBuffer, length );
            
            m_FilePos = m_FilePos + length;
        }
    }
    
    return writeCount;
}

int MemMapFilePersistence::Read( char* pBuffer, int length ){
    int readCount = 0;
    if( IsValid() ){
        std::lock_guard<std::mutex> al( m_LockFile );
        
        int readFrom = m_FilePos;
        readCount = readFrom + length < m_FileSize ? length : m_FileSize - readFrom;
        if( readCount > 0 ){
            memcpy( pBuffer, m_pMMapFile + readFrom, readCount );
            
            m_FilePos = m_FilePos + readCount;
        }
    }
    
    return readCount;
}

int MemMapFilePersistence::GetFileSize() const{
    if( m_FileID > 0 ){
        struct stat fileData;
        if( fstat( m_FileID, &fileData) == 0 ){
            return fileData.st_size;
        }else
            return -1;
    }
    else
        return -1;
}

}

#else
// TODO : windows version
#endif