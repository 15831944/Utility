/* 
 * File:   RollFilePersistence.cpp
 * Author: 0007989
 * 
 * Created on 2015年1月23日, 上午 9:15
 */

#include "RollFilePersistence.h"

#include "FilePersistence.h"
#include "MemMapFilePersistence.h"
#include <assert.h>
#include <string.h>
#include <cstdio>
#include "../StringTemplateUtil.h"

namespace KGI_TW_Der_Utility{ 


RollFilePersistence::RollFilePersistence(int rollSize) : m_RollSize(rollSize),m_LastIndex(0) {    
}

RollFilePersistence::~RollFilePersistence() {
}

bool RollFilePersistence::Open( const char* pFilename ){
    //assert( pFilename != NULL );
    if( pFilename == NULL )
        return false;
    
    if( m_CrtFile == NULL ){
        m_FilenamePattern = pFilename;
        m_LastIndex = AbstractFilePersistence::GetLastIndex(pFilename);
        
        m_CrtFile.reset( CreatePersistence() );
        return m_CrtFile->Open( pFilename );
    }
    return true;
}

void RollFilePersistence::Close(){
    std::lock_guard<std::mutex> al( m_LockFile );
    if( m_CrtFile != NULL ){
        m_CrtFile->Close();
        m_CrtFile.reset();
    }
}

AbstractFilePersistence* RollFilePersistence::CreatePersistence(){
    return new FilePersistence();
}

int RollFilePersistence::Write( const char* pBuffer, int length ){
    std::lock_guard<std::mutex> al( m_LockFile );
    if( m_CrtFile != NULL ){
        if( m_RollSize != 0 && m_CrtFile->GetSize() + length > m_RollSize * AbstractFilePersistence::MBSize ){
            // Change file
            m_CrtFile->Close();
            
            // Rename
            std::string lm;
            m_LastIndex++;
            StringTemplateUtil::Format( lm, "%s.%d", m_FilenamePattern.c_str(), m_LastIndex );
            int rcRename = rename( m_FilenamePattern.c_str(), lm.c_str() );
            if( rcRename != 0 )
            {
                // rename failed
            }
            
            m_CrtFile.reset( CreatePersistence() );
            if( m_CrtFile->Open( m_FilenamePattern.c_str() ) == false )
                return 0;
        }
        
        return m_CrtFile->Write( pBuffer, length );        
    }else
        return 0;
}

int RollFilePersistence::Read( char* pBuffer, int length ){
    std::lock_guard<std::mutex> al( m_LockFile );
    if( m_CrtFile != NULL ){
        return m_CrtFile->Read( pBuffer, length );
    }else
        return 0;
}

int RollFilePersistence::GetSize() const{
    if( m_CrtFile != NULL ){
        return m_CrtFile->GetSize();
    }else
        return 0;
}

AsyncRollFilePersistence::AsyncRollFilePersistence(int rollSizeMB, int flushPeriodMS ) : RollFilePersistence(rollSizeMB) {    
    m_SwitchBuffer.SetNotifyBufferFull( std::bind( &AsyncRollFilePersistence::OnNotifyBufferFull, this, std::placeholders::_1, std::placeholders::_2 ) );
    m_SwitchBuffer.SetTimerNotify( flushPeriodMS, true );
}

AsyncRollFilePersistence::~AsyncRollFilePersistence() {
    printf("~AsyncRollFilePersistence()\r\n");
}

void AsyncRollFilePersistence::Close(){
    m_SwitchBuffer.SetTimerNotify( 1000, false ); // stop notify timer
    //m_SwitchBuffer.Stop();
    m_SwitchBuffer.Flush();    
    
    RollFilePersistence::Close();
}

int AsyncRollFilePersistence::Write( const char* pBuffer, int length ){
    m_SwitchBuffer.WriteBuffer( pBuffer, length );
    
    return length;
}

void AsyncRollFilePersistence::OnNotifyBufferFull( const char* pBuffer, int length ){
    RollFilePersistence::Write( pBuffer, length );
}

#ifdef _OS_LINUX_
AbstractFilePersistence* MMapRollFilePersistence::CreatePersistence(){
    return new MemMapFilePersistence(m_RollSize * AbstractFilePersistence::MBSize);
}
#else
	// TODO
#endif
}