/* 
 * File:   AsyncFileLogger.cpp
 * Author: 0007989
 * 
 * Created on 2014年8月18日, 上午 8:31
 */

#include "AsyncFileLogger.h"

#include <string.h>
#include <memory>

namespace KGI_TW_Der_Utility{  
    
AsyncFileLogger::AsyncFileLogger( const char *pFilenamePattern, const char *pLoggerName ) : AsyncRollFileLogger(pFilenamePattern,0,pLoggerName){    
}

AsyncFileLogger::~AsyncFileLogger() {
}
    
//
//AsyncFileLogger::AsyncFileLogger(const std::string& filename ) : FileLogger(filename){
//    
//}   
//
//AsyncFileLogger::AsyncFileLogger(const std::string& filename, const std::string& loggerName ) : FileLogger( filename, loggerName ){
//    
//}
//
//AsyncFileLogger::~AsyncFileLogger() {
//}
//
//bool AsyncFileLogger::Open(){
//    m_ShareLock.reset( new int(10) );
//    if( FileLogger::Open() == false ){
//        m_ShareLock.reset();
//        return false;
//    }
//    
//    std::thread tWriteLog( &AsyncFileLogger::WriteLogThread, this );
//    tWriteLog.detach();
//    
//    return true;
//}
//
//void AsyncFileLogger::Close(){
//    while(true){
//        if( m_Queue.Size() <= 0 )
//            break;
//    }
//    m_ShareLock.reset();
//    FileLogger::Close();
//}
//
//void AsyncFileLogger::WriteLogString( const char* pMsg ){
//    if( pMsg != NULL ){
//        int size = strlen(pMsg) + 1;
//        std::shared_ptr<char> logData( new char[size] );
//        memcpy( logData.get(), pMsg, size );
//        char *pData = logData.get();
//        pData[size] = 0x00;
//        
//        m_Queue.Push( logData );
//    }
//}
//
//void AsyncFileLogger::WriteLogThread(){
//    std::weak_ptr<int> tmpLock( m_ShareLock );
//    while(true){
//        std::shared_ptr<char> logData = m_Queue.Pop();
//        if( logData != NULL ){
//            
//            // Get log file
//            std::shared_ptr<int> useFile( tmpLock.lock() );
//            if( useFile == NULL )
//            {
//                //printf( "AsyncFileLogger File locker closed, stop thread\r\n" ); // DEBUG
//                break;
//            }
//            
//            // Write log
//            FileLogger::WriteLogString( logData.get() );
//            //printf( "AsyncFileLogger Write one log\r\n" ); // DEBUG
//        }        
//    }
//}

}