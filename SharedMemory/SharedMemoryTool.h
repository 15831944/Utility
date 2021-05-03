/*
 * File:   SharedMemoryTool.h
 * Author: Andy Huang
 *
 * Created on 2019年9月23日, 下午 3:18
 */

#ifndef UTILITY_SHAREDMEMORYTOOL_H
#define UTILITY_SHAREDMEMORYTOOL_H

#ifdef _OS_LINUX_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "../CommonDef.h"

namespace KGI_TW_Der_Utility {

template<typename T>
struct SharedMemory {
    volatile int SeqNo;
    T Data;
};

template<typename T>
class SharedMemoryWriter {
public:
    SharedMemoryWriter( const char* shmName, size_t capacity ) : ShmName_( shmName ), Capacity_( capacity ) {}
    SharedMemoryWriter( const SharedMemoryWriter& other ) = delete;
    SharedMemoryWriter& operator=( const SharedMemoryWriter& other ) = delete;
    ~SharedMemoryWriter() = default;

    /// \brief Open(Create) shared memory file
    /// \param isCreate output whether the shared memory file is created
    /// \return Whether open succeeded
    bool Open( bool& isCreate ) {
        int shm_fd;
        if ( ( shm_fd = shm_open( ShmName_.c_str(), ( O_CREAT | O_EXCL | O_RDWR ), ( S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) ) ) > 0 ) {
            isCreate = true;
        }
        else if ( ( shm_fd = shm_open( ShmName_.c_str(), ( O_CREAT | O_RDWR ), ( S_IREAD | S_IWRITE ) ) ) > 0 ) {
            isCreate = false;
        }
        else {
            return false;
        }

        int shm_size = sizeof( SharedMemory<T> ) * Capacity_;
        ftruncate( shm_fd, shm_size );

        Data_ = (SharedMemory<T> *)mmap( 0, shm_size, ( PROT_READ | PROT_WRITE ), MAP_SHARED, shm_fd, 0 );
        if ( Data_ == MAP_FAILED ) {
            close( shm_fd );
            return false;
        }

        if ( isCreate ) {
            memset( Data_, 0, shm_size );
        }
        else {
            SharedMemory<T> *ptr;
            for ( size_t i = 0; i < Capacity_; i++ ) {
                ptr = (SharedMemory<T> *)Data_ + i;
                if ( ptr->SeqNo & 1 ) {
                    ptr->SeqNo++;
                }
            }
        }

        close( shm_fd );
        return true;
    }

    /// \brief Close shared memory file
    /// \return Whether close succeeded
    bool Close() {
        if ( Data_ == MAP_FAILED ) {
            return true;
        }
        if ( munmap( Data_, sizeof( SharedMemory<T> ) * Capacity_ ) == -1 ) {
            return false;
        }

        Data_ = (SharedMemory<T> *)MAP_FAILED;
        return true;
    }

    /// \brief Write data into certain index of shared memory
    /// \param index index of the data record
    /// \param data data to be inserted
    /// \return Whether write succeeded
    bool Write( size_t index, const T& data ) {
        if ( Data_ == NULL ) {
            return false;
        }

        if ( index >= Capacity_ ) {
            return false;
        }

        Data_[ index ].SeqNo++;
        asm volatile( "" : : : "memory" );
        memcpy( &Data_[ index ].Data, &data, sizeof( T ) );
        asm volatile( "" : : : "memory" );
        Data_[ index ].SeqNo++;

        return true;
    }
private:
    SharedMemory<T>* Data_;
    std::string ShmName_;
    size_t Capacity_;
};

template <typename T>
class SharedMemoryReader {
public:
    SharedMemoryReader( const char* shmName ) : ShmName_( shmName ) {}
    SharedMemoryReader( const SharedMemoryReader& other ) = delete;
    SharedMemoryReader& operator=( const SharedMemoryReader& other ) = delete;
    ~SharedMemoryReader() = default;

    /// \brief Open shared memory file
    /// \return Whether open succeeded
    bool Open() {
        int shm_fd;
        if ( ( shm_fd = shm_open( ShmName_.c_str(), ( O_RDONLY ), ( S_IREAD ) ) ) < 0 ) {
            return false;
        }

        struct stat sb;
        if ( fstat( shm_fd, &sb ) == -1 ) {
            close( shm_fd );
            return false;
        }

        Capacity_ = sb.st_size / sizeof( SharedMemory<T> );

        Data_ = (SharedMemory<T> *)mmap( 0, sb.st_size, ( PROT_READ ), MAP_SHARED, shm_fd, 0 );
        if ( Data_ == MAP_FAILED ) {
            close( shm_fd );
            return false;
        }

        close( shm_fd );
        return true;
    }

    /// \brief Close shared memory file
    /// \return Whether close succeeded
    bool Close() {
        if ( Data_ == MAP_FAILED ) {
            return true;
        }
        if ( munmap( Data_, sizeof( SharedMemory<T> ) * Capacity_ ) == -1 ) {
            return false;
        }

        Data_ = (SharedMemory<T> *)MAP_FAILED;
        return true;
    }

    /// \brief Read data at certain index from shared memory
    /// \param index index of data record
    /// \param data data read from specified index
    /// \return Whether read succeeded
    bool Read( size_t index, T& data ) {
        if ( Data_ == NULL ) {
            return false;
        }

        if ( index >= Capacity_ ) {
            return false;
        }

        while ( 1 ) {
            volatile int SeqNo1 = Data_[ index ].SeqNo;
            if ( SeqNo1 & 1 ) {
                continue;
            }
            asm volatile( "" : : : "memory" );
            memcpy( &data, &( Data_[ index ].Data ), sizeof( T ) );
            asm volatile( "" : : : "memory" );
            volatile int SeqNo2 = Data_[ index ].SeqNo;
            if ( SeqNo1 != SeqNo2 ) {
                continue;
            }
            break;
        }

        return true;
    }
private:
    SharedMemory<T>* Data_;
    std::string ShmName_;
    size_t Capacity_;
};

}

#endif

#endif /* UTILITY_SHAREDMEMORYTOOL_H */