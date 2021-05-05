/* 
 * File:   AsyncRollFileLogger.cpp
 * Author: 0007989
 * 
 * Created on 2014年8月18日, 上午 10:29
 */



#include "AsyncRollFileLogger.h"

#ifdef _OS_LINUX_
#include <dirent.h>
#else // _WIN32
#include <Windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LogMessage.h"
#include "../CommonDebug.h"

#include <stdarg.h>
#include "../StringUtil.h"
#include "RollFileLogger.h"

namespace ProgLab{ namespace Utility{  

AsyncRollFileLogger::AsyncRollFileLogger(  const char *pFilenamePattern,int maxSizeMB, const char *pLoggerName ) : 
    _BaseFileLogType_( std::string(pFilenamePattern), std::string("") ),m_MaxSizeMB(maxSizeMB),m_IsThreadReady(false)
    ,m_pBuffer1(NULL),m_pBuffer2(NULL),m_pCrtBuffer(NULL),m_CrtBufferPosition(0)
    
{
    m_LastIndex = 0;
    m_FilenamePattern = pFilenamePattern;
    
    m_LastIndex = RollFileLogger::GetLastIndex(m_FilenamePattern);
}

AsyncRollFileLogger::~AsyncRollFileLogger() {
    //assert( m_pBuffer1 == NULL && m_pBuffer2 == NULL && m_pCrtBuffer == NULL );
}

bool AsyncRollFileLogger::Open(){
    assert( m_pBuffer1 == NULL && m_pBuffer2 == NULL && m_pCrtBuffer == NULL );
    {
        std::unique_lock<std::mutex> al( m_FileOperation );
        bool result = _BaseFileLogType_::Open();
        if( result == false )
            return result;
    }
    
    // Crate buffer
    m_pBuffer1 = new char[m_BufferSize];
    m_pBuffer2 = new char[m_BufferSize];
    m_pCrtBuffer = m_pBuffer1;
    m_CrtBufferPosition = 0;
    
    if( !m_IsThreadReady ){
        m_IsThreadReady = true;
        std::thread tWriteLog( &AsyncRollFileLogger::WriteLogThread, this );
        tWriteLog.detach();
        
        std::thread tTimerWriteLog( &AsyncRollFileLogger::TimerWriteLogThread, this );
        tTimerWriteLog.detach();
    }
    
    return true;
}
    
void AsyncRollFileLogger::Close(){    
    while(true){
        if( m_Queue.Size() <= 0 )
            break;
    }
    
    m_DumpBuffer.Set(false); // stop timer for write log
    m_Queue.Abort(); // notify WriteLogThread() to stop     
    
    // Write the last log
    if( m_pCrtBuffer != NULL && m_CrtBufferPosition > 0 ){
        _BaseFileLogType_::WriteBlock( m_pCrtBuffer, m_CrtBufferPosition );
    }
    
    std::unique_lock<std::mutex> al( m_FileOperation );
    _BaseFileLogType_::Close();
    
    delete[] m_pBuffer1;
    delete[] m_pBuffer2;
    m_pBuffer1 = m_pBuffer2 = m_pCrtBuffer = NULL;
}

void AsyncRollFileLogger::WriteLogString( const char* pMsg ){
    //std::lock_guard<std::mutex> al( m_BufferOperation );          // will double-lock with SwitchBuffer())
#ifdef _ASYNCROLLFILELOGGER_H_Use_Buffer_
    if( pMsg != NULL ){
        int size = strlen(pMsg);
        
        if( m_CrtBufferPosition + size >= m_BufferSize ){
            SwitchBuffer();
        }
        
        {
            std::lock_guard<std::mutex> al( m_BufferOperation );
            memcpy( m_pCrtBuffer + m_CrtBufferPosition, pMsg, size );
            m_CrtBufferPosition += size;
        }
    }
#else
    if( pMsg != NULL ){
        int size = strlen(pMsg) + 1;
        
        char* ptmp = new char[size];        
        memcpy( ptmp, pMsg, size );
        ptmp[size-1] = 0x00;
        
        std::shared_ptr<char> logData( ptmp,std::default_delete<char[]>() );
        
        m_Queue.Push( logData );
    }
#endif
}

void AsyncRollFileLogger::WriteLogThread(){
    
    while(true){                
        try{
#ifdef _ASYNCROLLFILELOGGER_H_Use_Buffer_
            WriteBufferData logData = m_Queue.Pop();
            if( logData.pBuffer != NULL && logData.Size > 0 ){
                std::unique_lock<std::mutex> al( m_FileOperation );
                // Write log
                _BaseFileLogType_::WriteBlock( logData.pBuffer, logData.Size );
#else
            std::shared_ptr<char> logData = m_Queue.Pop();
            if( logData != NULL ){

                std::unique_lock<std::mutex> al( m_FileOperation );

                // Write log
                _BaseFileLogType_::WriteLogString( logData.get() );
                //printf( "AsyncFileLogger Write one log, Queue(%d) %s\r\n", m_Queue.Size(), logData.get() ); // DEBUG
#endif
                if( m_MaxSizeMB > 0 ){
                    // Check the file size
                    long int fileSize = GetSize();            
                    if( fileSize > m_MaxSizeMB * RollFileLogger::MBSize )
                    {
                        // Switch file
                        _BaseFileLogType_::Close(); // use Close() from base class, otherwise will dead lock

                        // Rename
                        LogMessage lm;
                        m_LastIndex++;
                        lm.Format( "%s.%d", m_FilenamePattern.c_str(), m_LastIndex );
                        int rcRename = rename( m_FilenamePattern.c_str(), lm.GetMessage() );
                        if( rcRename != 0 )
                        {
                            // rename failed
                        }else
                        {
                            if( _BaseFileLogType_::Open() == false ) // use Open() from base class, otherwise will dead lock
                            {
                                break; // stop
                            }
                        }
                    }      
                }
            }
        }catch( BQThreadAbort exp ){
            break; // thread terminated
        }catch( ... ){

        }                       
    }// end while
}
    
#ifdef _ASYNCROLLFILELOGGER_H_Use_Buffer_
void AsyncRollFileLogger::TimerWriteLogThread(){
    while(true){
        if( m_DumpBuffer.Wait( 1000 ) == false ){ // timeout, then write buffer            
            SwitchBuffer();
        }else
            break; // stopped
    }
}

void AsyncRollFileLogger::SwitchBuffer(){
    std::lock_guard<std::mutex> al( m_BufferOperation );
            
    WriteBufferData wbData;
    wbData.Size = m_CrtBufferPosition;
    wbData.pBuffer = m_pCrtBuffer;
    m_Queue.Push( wbData );

    m_CrtBufferPosition = 0;
    if( m_pCrtBuffer == m_pBuffer1 )
        m_pCrtBuffer = m_pBuffer2;
    else
        m_pCrtBuffer = m_pBuffer1;
}
#endif

AsyncRollFileLoggerEx::AsyncRollFileLoggerEx(const char *pFilenamePattern,int maxSizeMB, const char *pLoggerName){
    m_LoggerName = pLoggerName;
    m_pPersistence = new AsyncRollFilePersistence(maxSizeMB);
    m_FilenamePattern = pFilenamePattern;
}

AsyncRollFileLoggerEx::~AsyncRollFileLoggerEx(){
    std::lock_guard<std::mutex> al( m_LockPersistence );
    delete m_pPersistence;
    m_pPersistence = NULL;
} 

bool AsyncRollFileLoggerEx::Open(){
    std::lock_guard<std::mutex> al( m_LockPersistence );
    if( m_pPersistence != NULL ){
        return m_pPersistence->Open( m_FilenamePattern.c_str() );
    }else
        return false;
}
    
void AsyncRollFileLoggerEx::Close(){
    std::lock_guard<std::mutex> al( m_LockPersistence );
    if( m_pPersistence != NULL ){
        m_pPersistence->Close();
    }
}

void AsyncRollFileLoggerEx::WriteLogString( const char* pMsg ){
    std::lock_guard<std::mutex> al( m_LockPersistence );
    if( m_pPersistence != NULL && pMsg != NULL ){
        int length = strlen( pMsg );
        m_pPersistence->Write( pMsg, length );
    }
}

/* =========== DeferedAsyncRollFileLoggerEx =========== */

DeferedAsyncRollFileLoggerEx::DeferedAsyncRollFileLoggerEx(const char *pFilenamePattern,int maxSizeMB, const char *pLoggerName){
    m_LoggerName = pLoggerName;
    m_pPersistence = new AsyncRollFilePersistence(maxSizeMB);
    m_FilenamePattern = pFilenamePattern;
}

DeferedAsyncRollFileLoggerEx::~DeferedAsyncRollFileLoggerEx(){
    std::lock_guard<std::mutex> al( m_LockPersistence );
    delete m_pPersistence;
    m_pPersistence = NULL;
} 

bool DeferedAsyncRollFileLoggerEx::Open(){
    std::lock_guard<std::mutex> al( m_LockPersistence );
    if( m_pPersistence != NULL ){
        return m_pPersistence->Open( m_FilenamePattern.c_str() );
    }else
        return false;
}
    
void DeferedAsyncRollFileLoggerEx::Close(){
    std::lock_guard<std::mutex> al( m_LockPersistence );
    if( m_pPersistence != NULL ){
        m_pPersistence->Close();
    }
}

void DeferedAsyncRollFileLoggerEx::WriteLogString( const char* pMsg ){
    std::lock_guard<std::mutex> al( m_LockPersistence );
    if( m_pPersistence != NULL && pMsg != NULL ){
        int length = strlen( pMsg );
        m_pPersistence->Write( pMsg, length );
    }
}
    
}

