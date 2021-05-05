/* 
 * File:   FilePersistence.h
 * Author: 0007989
 *
 * Created on 2015年1月26日, 上午 8:19
 */

#include "AbstractFilePersistence.h"
#include <mutex>
#include <assert.h>

#ifndef UTILITY_FILEPERSISTENCE_H
#define	UTILITY_FILEPERSISTENCE_H

namespace ProgLab{ namespace Utility{ 

    /// \brief 一般檔案型式的Persistence類別
    /// 使用的是基本的C的檔案函式庫
class FilePersistence : public AbstractFilePersistence{
public:
    FilePersistence();
    FilePersistence(const FilePersistence& orig) = delete;
    virtual ~FilePersistence();
    
    virtual bool Open( const char* pFilename );
    virtual void Close();
    
    virtual int Write( const char* pBuffer, int length );
    virtual int Read( char* pBuffer, int length );
    
    /// \return 目前的位置，失敗時傳回-1
    virtual int SeekFromHead( int offset );
    /// \return 目前的位置，失敗時傳回-1
    virtual int SeedFromTail( int offset );
    
    /// \return 檔案大小，如果無法讀取時，則傳回-1
    virtual int GetSize() const;
    
protected:    
    FILE *m_FileID;    
    mutable std::mutex m_LockFile;
};

}
#endif	/* UTILITY_FILEPERSISTENCE_H */

