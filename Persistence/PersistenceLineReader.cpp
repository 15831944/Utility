/* 
 * File:   PersistenceLineReader.cpp
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 * 
 * Created on 2016年7月20日, 下午 5:22
 */

#include "PersistenceLineReader.h"

#include <memory.h>

namespace KGI_TW_Der_Utility{ 

PersistenceLineReader::PersistenceLineReader() {
}

PersistenceLineReader::PersistenceLineReader(const PersistenceLineReader& orig) {
}

PersistenceLineReader::~PersistenceLineReader() {
}

void PersistenceLineReader::Read( AbstractFilePersistence* pPersistence, std::function<void(const std::string&)> lineFeed ){
    const int bufferSize = 2048;
    char buffer[bufferSize];
    //char delim = '\r';
    //int delimOffset = 1; // for skip \r\n
    int bufferOffset = 0;
    int dataLength = 0;
    while(true){
        dataLength = pPersistence->Read( buffer + bufferOffset, bufferSize - bufferOffset );

        dataLength += bufferOffset;

        if( dataLength <= 0 )
            break;

        int startPos = 0;
        std::string line;
        for( int idx = 0; idx < dataLength; ++idx ){
            if( buffer[idx] == '\r' || buffer[idx] == '\n' ){
                if( idx - startPos > 0 ){
                    line.assign( &buffer[startPos], &buffer[idx] );

                    // Process line
                    if( line.empty() == false ){
                        lineFeed( line );
                    }
                }                    

                startPos = idx + 1;
            }
        }  

        if( startPos < dataLength ){
            // remain data
            bufferOffset = dataLength - startPos;
            memcpy( buffer, buffer+startPos, dataLength - startPos );                
        }
    } 
}

}