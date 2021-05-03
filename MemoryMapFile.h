/* 
 * File:   MemoryMapFile.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2016年2月4日, 上午 11:28
 */

#ifndef UTILITY_MEMORYMAPFILE_H
#define	UTILITY_MEMORYMAPFILE_H

#include <string>
#include <sys/types.h>
#ifdef _OS_LINUX_
#include <sys/ipc.h>
#include <sys/mman.h>
#else
#endif
#include <fcntl.h>
#include <mutex>

namespace KGI_TW_Der_Utility{ 

#ifdef _OS_LINUX_
/// \brief 建立或開啟一份指定名稱的檔案，並建立MemoryMap模式來進行存取
class MemoryMapFile {
public:
    /// \remark will throw std::runtime_error when error
    MemoryMapFile(const std::string& filename, int filesize, int flags, mode_t mode );
    MemoryMapFile(const MemoryMapFile& orig) = delete;
    virtual ~MemoryMapFile();
    
    int Write( const char* pData, int length, int offset = 0);
    int Read( char* pBuffer, int length, int offset = 0);    
    
private:
    int     m_FileID;
    int     m_FileSize;
    char*   m_pMMapFile;
    
    mutable std::mutex m_LockFile;
};
#else
#endif

}
#endif	/* UTILITY_MEMORYMAPFILE_H */

