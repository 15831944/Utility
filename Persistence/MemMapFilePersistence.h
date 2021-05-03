/* 
 * File:   MemMapFilePersistence.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2016年2月16日, 上午 11:32
 */

#include "AbstractFilePersistence.h"
#include <mutex>
#include <assert.h>
#ifdef _OS_LINUX_
#ifdef _CYGWIN_
#include <atomic>
#else
#if __cplusplus < 201103L
#include <cstdatomic>
#else
#include <atomic>
#endif // __cplusplus
#endif
#include <sys/mman.h>
#else
#endif

#ifndef UTILITY_MEMMAPFILEPERSISTENCE_H
#define	UTILITY_MEMMAPFILEPERSISTENCE_H

namespace KGI_TW_Der_Utility{ 

#ifdef _OS_LINUX_
/// \brief 使用MemoryMap file的方式來寫入Log
/// 使用這種方式寫Log的話，目前是無法用tail -f <file>來即時看資料的
class MemMapFilePersistence : public AbstractFilePersistence{
public:
    MemMapFilePersistence(int fileSize);
    MemMapFilePersistence(const MemMapFilePersistence& orig) = delete;
    virtual ~MemMapFilePersistence();

    virtual bool Open( const char* pFilename );
    virtual void Close();
    
    virtual int Write( const char* pBuffer, int length );
    virtual int Read( char* pBuffer, int length );
    
    /// \return 目前的位置，失敗時傳回-1
    virtual int SeekFromHead( int offset );
    /// \return 目前的位置，失敗時傳回-1
    virtual int SeedFromTail( int offset );
    
    /// \return 檔案大小，如果無法讀取時，則傳回-1
    virtual int GetSize() const{ return m_FilePos; }
    
    bool IsValid() const { return m_FileID > 0 && m_pMMapFile != MAP_FAILED; }
    
protected:    
    int                 m_FileID;    
    int                 m_FileSize;         //!< 預設的檔案大小
    mutable std::mutex  m_LockFile;
    std::atomic<int>    m_FilePos;    
    char*               m_pMMapFile;
    
    int                 m_OrgFileSize;      //!< 原本開啟檔案時的大小
    
protected:
    /// \brief 取得實際檔案的大小
    /// \return 檔案大小，如果無法讀取時，則傳回-1
    int GetFileSize() const;

};
#else
	// TODO : windows version?
#endif

}
#endif	/* UTILITY_MEMMAPFILEPERSISTENCE_H */

