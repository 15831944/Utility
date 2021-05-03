/* 
 * File:   FilePersistence.cpp
 * Author: 0007989
 * 
 * Created on 2015年1月26日, 上午 8:19
 */

#include "FilePersistence.h"
#include <stdio.h>

namespace KGI_TW_Der_Utility{ 

FilePersistence::FilePersistence() : m_FileID(NULL) {
}

FilePersistence::~FilePersistence() {
    Close();
}

bool FilePersistence::Open( const char* pFilename ){
#ifdef _OS_LINUX_
    m_FileID = ::fopen( pFilename, "a+");    
#else // _WIN32
    m_FileID = ::_fsopen( pFilename, "a+", _SH_DENYWR);
#endif   
    return m_FileID != NULL;
}

void FilePersistence::Close(){
    if( m_FileID != NULL )
    {
        ::fclose( m_FileID );
        m_FileID = NULL;
    }
}

int FilePersistence::SeekFromHead( int offset ){
    if( m_FileID != NULL ){
        return ::fseek( m_FileID, SEEK_SET, offset );
    }else
        return -1;
}
    
int FilePersistence::SeedFromTail( int offset ){
    if( m_FileID != NULL ){
        return ::fseek( m_FileID, SEEK_END, offset );
    }else
        return -1;
}

int FilePersistence::Write( const char* pBuffer, int length ){
    //assert( m_FileID != NULL );
    if( m_FileID == NULL )
        return 0;
    
    int writeCount = 0;
    if( m_FileID != NULL ){
        std::lock_guard<std::mutex> al( m_LockFile );
        
        writeCount = ::fwrite( pBuffer, length, 1, m_FileID );
        ::fflush( m_FileID );        
    }
    
    return writeCount;
}

int FilePersistence::Read( char* pBuffer, int length ){
    //assert( m_FileID != NULL );
    if( m_FileID == NULL )
        return 0;
    
    int readCount = 0;
    if( m_FileID != NULL ){
        std::lock_guard<std::mutex> al( m_LockFile );
        
        readCount = ::fread( (void *)pBuffer, 1, length, m_FileID );
        ::fflush( m_FileID );
    }
    
    return readCount;
}

int FilePersistence::GetSize() const{
    if( m_FileID != NULL )
        return ftell( m_FileID );
    else
        return -1;
}

}