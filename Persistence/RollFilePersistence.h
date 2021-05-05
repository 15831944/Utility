/* 
 * File:   RollFilePersistence.h
 * Author: 0007989
 *
 * Created on 2015年1月23日, 上午 9:15
 */

#ifndef UTILITY_ROLLFILEPERSISTENCE_H
#define	UTILITY_ROLLFILEPERSISTENCE_H

#include <string>
#include <mutex>
#include <memory>
#include <string.h>
#include "AbstractFilePersistence.h"
#include "../SwitchBuffer.h"

namespace ProgLab{ namespace Utility{ 


class RollFilePersistence : public AbstractFilePersistence{
public:
    RollFilePersistence(int rollSize = 0);
    RollFilePersistence(const RollFilePersistence& orig) = delete;
    virtual ~RollFilePersistence();
    
    virtual bool Open( const char* pFilename );
    virtual void Close();
    
    virtual int Write( const char* pBuffer, int length );
    virtual int Read( char* pBuffer, int length );
    
    virtual int GetSize() const;    
protected:
    int m_RollSize;
    int m_LastIndex;    
    std::string m_FilenamePattern;
    std::shared_ptr<AbstractFilePersistence> m_CrtFile;
    mutable std::mutex m_LockFile;
    
protected:
    virtual AbstractFilePersistence* CreatePersistence();
};

class MMapRollFilePersistence : public RollFilePersistence {
public:
    MMapRollFilePersistence(int rollSize = 0) : RollFilePersistence(rollSize){}
    MMapRollFilePersistence(const MMapRollFilePersistence& orig) = delete;
    virtual ~MMapRollFilePersistence(){}
    
protected:
    virtual AbstractFilePersistence* CreatePersistence();
};

class AsyncRollFilePersistence : public RollFilePersistence{
public:
    /// \param rollSize 每個檔案的最大位元組數，單位為MB
    /// \param flushPeriodMS 每隔指定的minisecond就將buffer中的資料寫入檔案
    AsyncRollFilePersistence( int rollSizeMB = 0, int flushPeriodMS = 1000 );
    AsyncRollFilePersistence(const AsyncRollFilePersistence& orig) = delete;
    virtual ~AsyncRollFilePersistence();
    
    virtual int Write( const char* pBuffer, int length );
    
    virtual void Close();
protected:
    SwitchBuffer m_SwitchBuffer;
protected:
    void OnNotifyBufferFull( const char* pBuffer, int length );
    
};


}
#endif	/* UTILITY_ROLLFILEPERSISTENCE_H */

