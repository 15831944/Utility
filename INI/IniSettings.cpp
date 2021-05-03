/* 
 * File:   IniSettings.cpp
 * Author: 0007989
 * 
 * Created on 2014年3月7日, 下午 7:18
 */

#include "IniSettings.h"
#include <string.h>
//#include <regex.h>
#include "../Log/LogMessage.h"
#include "IniSection.h"
#include "IniKeyValue.h"
#include "IniComment.h"
//#include "../WorkingBuffer.h"
#include "../MovingBuffer.h"

namespace KGI_TW_Der_Utility
{   
    
 const char IniSettings::m_PtnSection[50] = "^\\[(.*)\\]";
 const char IniSettings::m_PtnKeyValue[50] = "^([^#]*)=([^#]*)";

IniSettings::IniSettings(int readBufferSize) : m_ReadBufferSize(readBufferSize) {
    m_pCrtSection = NULL;
    int rc = m_RegSection.Compile( m_PtnSection );
    if( rc != 0 )
        throw "[IniSettings] Prepare regular express failed";
    rc = m_RegKeyValue.Compile( m_PtnKeyValue );
    if( rc != 0 )
        throw "[IniSettings] Prepare regular express failed";
}

IniSettings::~IniSettings() {
    //::regfree( &m_RegSection );
    //::regfree( &m_RegKeyValue );
}

bool IniSettings::Load( const std::string& filename ){
    m_Filename = filename;
    
    FILE *FileID;
#ifdef _OS_LINUX_
    FileID = ::fopen( m_Filename.c_str(), "r");
#else // _WIN32
	::fopen_s(&FileID, m_Filename.c_str(), "r");
#endif
    if( FileID == NULL )
        return false;    
    
    int bufferSize = m_ReadBufferSize;
    MovingBuffer workBuffer( bufferSize );
        
    int available = 0;
    while(true){
        char *pBuffer = workBuffer.GetBuffer() + workBuffer.GetPosition();
        available = workBuffer.GetTotalSize() - workBuffer.GetPosition();
        ::memset( pBuffer, 0, available );
        int readCount = ::fread( pBuffer, 1, available, FileID );
        
        pBuffer = workBuffer.GetBuffer();           // reset to head
        readCount += workBuffer.GetPosition();      // calculate the total buffer( read + remained )
        // if have cached data, then set the parsing buffer to the begin of working buffer
        if( readCount <= 0 ) 
            break;      // no more data
        
        int usedSize = Parse( pBuffer, readCount );
        
        // Process remained data
        if( usedSize < readCount )
        {
            workBuffer.SetPosition(0);
            workBuffer.CacheData( pBuffer + usedSize, readCount - usedSize );            
        }else
            workBuffer.SetPosition(0);
    }
    
    // Put this last section into IniFile
    if( m_pCrtSection != NULL )
    {
        m_IniFile.AddSection( *m_pCrtSection );
        delete m_pCrtSection;
    }            
    
    ::fclose( FileID );
    
    return true;
}

/*
bool IniSettings::Load( const std::string& filename ){
    m_Filename = filename;
    
    FILE *FileID;
#ifdef _OS_LINUX_
    FileID = ::fopen( m_Filename.c_str(), "r");
#else // _WIN32
	::fopen_s(&FileID, m_Filename.c_str(), "r");
#endif
    if( FileID == NULL )
        return false;    
    
    int bufferSize = m_ReadBufferSize;
    WorkingBuffer workBuffer( bufferSize );
    char *pBuffer = workBuffer.GetData();
    int parseBufferSize = 0;
    
    while(true){
    
        ::memset( pBuffer, 0, bufferSize );
        int readCount = ::fread( pBuffer, 1, bufferSize, FileID );
        
        // if have cached data, then set the parsing buffer to the begin of working buffer
        if( workBuffer.GetUsedSize() > 0 ) 
        {
            workBuffer.ReleaseData();
            pBuffer = workBuffer.GetData();
            parseBufferSize = workBuffer.GetUsedSize() + readCount;
            workBuffer.SetUsedSize( parseBufferSize );
        }
        else
            parseBufferSize = readCount;
        
        int usedSize = Parse( pBuffer, parseBufferSize );
        
        // Process remained data
        if( usedSize < parseBufferSize )
        {
            if( usedSize <= 0 ) // the whole buffer is not used, then we need to extend the working buffer
            {                
                workBuffer.Extend( bufferSize );                 
                workBuffer.SetUsedSize( readCount );
                workBuffer.ReleaseData();
                pBuffer = workBuffer.GetData() + bufferSize;
            }
            else
            {
                // cache the remained data
                workBuffer.ReleaseData();
                workBuffer.ChangeBufferUsage( usedSize );
                pBuffer = workBuffer.GetData() + workBuffer.GetUsedSize();
            }
        }
        
        if( readCount < bufferSize )
            break; // End of file        
    }
    
    // Put this last section into IniFile
    if( m_pCrtSection != NULL )
    {
        m_IniFile.AddSection( *m_pCrtSection );
        delete m_pCrtSection;
    }            
    
    ::fclose( FileID );
    
    return true;
}*/

int IniSettings::Parse(const char* pBuffer, int length){
    
    LogMessage lm;
    int prevIndex = 0;
    int rc = 0;
    const int maxGroup = 3;
    //regmatch_t matchedData[maxGroup];
    std::vector<std::string> matchedData;
    std::string foundStr, key, value;
    
    for( int idx = 0; idx < length; ++idx )
    {
        if( pBuffer[idx] == 0x0a || pBuffer[idx] == 0x10 )
        {
            // Do parse
            int strLen = idx - prevIndex;
            if( strLen <= 0 ){
                prevIndex = idx + 1;
                continue; // Previous char is also end line character
            }
            
            std::string tmp;
            tmp.assign( pBuffer+prevIndex, strLen );
            //const char *pCrtBuffer = tmp.c_str();
            //printf( "%s\r\n", tmp.c_str() );
            matchedData.clear();
            rc = m_RegSection.Match( tmp.c_str(), maxGroup, matchedData );            
            if( rc == 0 ) // Match a section
            {
                //foundStr.assign( pCrtBuffer + matchedData[1].rm_so, matchedData[1].rm_eo - matchedData[1].rm_so );
                foundStr = matchedData[1];             
                
                // Put into IniFile
                if( m_pCrtSection != NULL )
                {
                    m_IniFile.AddSection( *m_pCrtSection );
                    delete m_pCrtSection;
                    m_pCrtSection = NULL;
                }
                
                m_pCrtSection = new IniSection( foundStr.c_str() );                
            }
            else
            {
                rc = m_RegKeyValue.Match( tmp.c_str(), maxGroup,matchedData );                
                if( rc == 0 )
                {
                    foundStr = matchedData[0];
                    key = matchedData[1];
                    value = matchedData[2];                    
                    
                    // Put Key/Value into IniFile
                    IniKeyValue kv( key.c_str(), value.c_str() ); 
                    if( m_pCrtSection != NULL )
                        m_pCrtSection->AddElement( kv );
                }
                else if( tmp.size() > 0 && tmp[0] == '#' )
                {
                    // comment
                    IniComment comment( tmp );
                    if( m_pCrtSection != NULL )
                        m_pCrtSection->AddElement( comment );
                }
                else
                {					
                    lm.Format( "[IniSettings] Unknown %s", tmp.c_str());
                    printf( "%s\r\n", lm.GetMessage());
                }
            }
            
            prevIndex = idx+1;
            
            continue;
        }
    }
    
    // remain data
    return prevIndex;
}


IniFile& IniSettings::GetIniFile(){
    return m_IniFile;
}

const IniFile& IniSettings::GetIniFile() const{
    return m_IniFile;
}

}

