/* 
 * File:   FileCache.cpp
 * Author: 0007989
 * 
 * Created on 2014年5月6日, 下午 12:44
 */

#include "FileCache.h"

#include <string.h>
#include <fstream>
#include <functional>
#include "MovingBuffer.h"

namespace KGI_TW_Der_Utility{   

FileCache::FileCache() {
    m_FileID = NULL;
}

FileCache::FileCache(const FileCache& orig) {
    m_FileID = NULL;
}

FileCache::~FileCache() {
    Close();
}

bool FileCache::Open( const char* pFilename ){
    m_CacheDispatcher.Start(this);
    
    m_Data.clear();
    
    m_File.open( pFilename,std::fstream::in );
    if( !m_File.fail() )
    {
        Restore();
    }
    m_File.close();
    
    m_File.open( pFilename,std::fstream::out | std::fstream::app );
    if( m_File.fail() ){
        return false;
    }
    else{        
        //m_File.seekp( 0, std::fstream::end ); // move to file end for appending
        return true;
    }
}
    
void FileCache::Close(){
    m_CacheDispatcher.Stop();

    m_File.close();
}    

int FileCache::Size(){
    if( !m_File.fail() )
    {
        return m_File.tellp();
    }
    else
        return 0;
}
    
void FileCache::Save( const char *pData ){
    // Copy
    int length = strlen(pData) + 1;
    if( length <= 1 )
        return; // no data
    char *pCopy = new char[length];
#ifdef _OS_LINUX_
    strncpy( pCopy, pData,length );
#else
	strcpy_s(pCopy, length, pData );
#endif
    pCopy[length-1] = 0x00; // end of string
    
    // Add 
    m_CacheDispatcher.Add( pCopy );
}

void FileCache::Restore(){
    if( m_File.good() ){
        std::string line;
        while( !m_File.eof() ){
            std::getline( m_File, line );
            if( line.size() > 0 )
            {
                // remove tail \r
                if( line[line.size()-1] == '\r' )
                    line = line.substr( 0, line.size()-1 );
                m_Data.push_back( line );
            }
            //else  // 2014/12/11 Justin , if read empty line, and clear the eofbit, then it will not read those lines after blank line
            //    m_File.clear( std::fstream::eofbit ); // must clear failbit when got nothing
        }
    }
}

const std::list<std::string>& FileCache::GetRestoredData(){
    return m_Data;
}

void FileCache::OnReceiveData( char *& data ){    
    if( !m_File.fail() )
    {
        m_File.write( data, strlen(data) );
        m_File.write( "\r\n", 2 );
        m_File.flush();
    }
    else
    {
        std::fstream::iostate stat = m_File.rdstate();
        if( stat & std::fstream::badbit )
            printf( "File is bad\r\n");
        if( stat & std::fstream::failbit )
            printf( "File is fail\r\n");
        if( stat & std::fstream::eofbit )
            printf( "File is eof\r\n");
        
    }
}



FileCacheEx::FileCacheEx() {    
    m_Buffers.SetNotifyBufferFull( std::bind( &FileCacheEx::OnWriteBuffer,this,std::placeholders::_1, std::placeholders::_2));
    m_Buffers.SetTimerNotify( 1000, true );
}

FileCacheEx::FileCacheEx(const FileCacheEx& orig) {    
    m_Buffers.SetNotifyBufferFull( std::bind( &FileCacheEx::OnWriteBuffer,this,std::placeholders::_1, std::placeholders::_2));
    m_Buffers.SetTimerNotify( 1000, true );
}

FileCacheEx::~FileCacheEx() {
    Close();
}

bool FileCacheEx::Open( const char* pFilename ){    
    
    m_Data.clear();
    
    m_File.open( pFilename,std::fstream::in );
    if( !m_File.fail() )
    {
        Restore();
    }
    m_File.close();
    
    m_File.open( pFilename,std::fstream::out | std::fstream::app );
    if( m_File.fail() ){
        return false;
    }
    else{        
        //m_File.seekp( 0, std::fstream::end ); // move to file end for appending
        return true;
    }
}
    
void FileCacheEx::Close(){
    //m_Buffers.SetTimerNotify( 0, false );   // stop timer 
    //m_Buffers.Stop();
    //sleep(1);                               // wait 1 sec for thread to abort
    m_File.close();
}    

int FileCacheEx::Size(){
    if( !m_File.fail() )
    {
        return m_File.tellp();
    }
    else
        return 0;
}
    
void FileCacheEx::Save( const char *pData ){
    if( pData == NULL )
        return;
    // Copy
    int length = strlen(pData);
    if( length <= 1 )
        return; // no data    
    
    // Add 
    {
        std::lock_guard<std::mutex> al( m_LockSaveOperation );
        int writeLen = m_Buffers.WriteBuffer( pData, length );
        writeLen = m_Buffers.WriteBuffer( "\r\n", 2 );
    }
    
}

void FileCacheEx::Restore(){
    if( m_File.good() ){
        std::string line;
        while( !m_File.eof() ){
            std::getline( m_File, line );
            if( line.size() > 0 )
            {
                // remove tail \r
                if( line[line.size()-1] == '\r' )
                    line = line.substr( 0, line.size()-1 );
                m_Data.push_back( line );
            }
            //else  // 2014/12/11 Justin , if read empty line, and clear the eofbit, then it will not read those lines after blank line
            //    m_File.clear( std::fstream::eofbit ); // must clear failbit when got nothing
        }
    }
}

const std::list<std::string>& FileCacheEx::GetRestoredData(){
    return m_Data;
}

void FileCacheEx::OnWriteBuffer( const char* pData, int length ){
    if( !m_File.fail() )
    {
        m_File.write( pData, length );        
        m_File.flush();
    }
    else
    {
        std::fstream::iostate stat = m_File.rdstate();
        if( stat & std::fstream::badbit )
            printf( "File is bad\r\n");
        if( stat & std::fstream::failbit )
            printf( "File is fail\r\n");
        if( stat & std::fstream::eofbit )
            printf( "File is eof\r\n");
        
    }
}

}