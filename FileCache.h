/* 
 * File:   FileCache.h
 * Author: 0007989
 *
 * Created on 2014年5月6日, 下午 12:44
 */

#ifndef UTILITY_FILECACHE_H
#define	UTILITY_FILECACHE_H

#include "./MT/DataDispatcher.h"
#include <list>
#include <string>
#include <fstream>
#include "SwitchBuffer.h"

namespace ProgLab{ namespace Utility{      

/// \brief Use another thread to save data into file
class FileCache : public DataDispatcher<char *>::DataDispatcherReceiver {
public:
    FileCache();
    FileCache(const FileCache& orig);
    virtual ~FileCache();
    
    /// \brief open file cache with given filename
    ///        When file is opened successfully, it will restore each line from the file and set the cursor to the end of file
    /// \param pFilename the filename of cached file
    /// \return success or failed    
    bool Open( const char* pFilename );
    void Close();    
    int Size();
    void Save( const char *pData );
    const std::list<std::string>& GetRestoredData();
    
    virtual void OnReceiveData( char *& data );
private:
    FILE *m_FileID;
    DataDispatcher<char *> m_CacheDispatcher;       //!< for writing cache data into buffer by other thread
    std::list<std::string> m_Data;
    
    std::fstream m_File;
    
private:
    void Restore();
};

/// \brief Use another thread to save data into file
class FileCacheEx {
public:
    FileCacheEx();
    FileCacheEx(const FileCacheEx& orig);
    virtual ~FileCacheEx();
    
    /// \brief open file cache with given filename
    ///        When file is opened successfully, it will restore each line from the file and set the cursor to the end of file
    /// \param pFilename the filename of cached file
    /// \return success or failed    
    bool Open( const char* pFilename );
    void Close();    
    int Size();
    void Save( const char *pData );
    const std::list<std::string>& GetRestoredData();
private:
    //FILE *m_FileID;    
    std::list<std::string> m_Data;
    SwitchBuffer m_Buffers;
    std::fstream m_File;
    mutable std::mutex m_LockSaveOperation;
    
private:
    void Restore();
    void OnWriteBuffer( const char* pData, int length );
};


}
#endif	/* UTILITY_FILECACHE_H */

