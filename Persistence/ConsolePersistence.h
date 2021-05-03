/* 
 * File:   ConsolePersistence.h
 * Author: 0007989
 *
 * Created on 2015年1月26日, 上午 8:16
 */

#ifndef UTILITY_CONSOLEPERSISTENCE_H
#define	UTILITY_CONSOLEPERSISTENCE_H

#include "AbstractFilePersistence.h"
#include <string.h>
#include <cstdio>
#include <memory>

namespace KGI_TW_Der_Utility{ 

    /// \brief 標準輸出的Persistence類別
class ConsolePersistence : public AbstractFilePersistence{
public:
    virtual bool Open( const char* pFilename ){ return true; }
    virtual void Close(){}
    
    virtual int Write( const char* pBuffer, int length ){
        std::unique_ptr<char[]> tmp( new char[length+1] );  // 20190215 解決valgrind會回報 Mismatched free()的問題，改用 char[]
        memcpy( tmp.get(), pBuffer, length );
        *(tmp.get()+length) = '\x0';
        printf( "%s", tmp.get() );
        return length;
    }
    virtual int Read( char* pBuffer, int length ){ return 0; }
    
    virtual int GetSize() const { return 0; }
};


}
#endif	/* UTILITY_CONSOLEPERSISTENCE_H */

