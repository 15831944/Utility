/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MemoryAllocator.h
 * Author: Justin Lin
 *
 * Created on 2017年3月14日, 上午 8:30
 */

#ifndef UTILITY_MEMORYALLOCATOR_H
#define UTILITY_MEMORYALLOCATOR_H

#include <list>
#include <mutex>
#include <queue>
#include <string>

namespace ProgLab{ namespace Utility{

/// \brief 記憶體區塊配置管理器
/// 可以自行配置一大塊記憶體，並依設定切成數小塊給其它人來配置及使用<br>
/// 此類別為虛擬基底類別，需再衍生實作才能實際使用
class MemoryAllocator{
public:
    virtual ~MemoryAllocator(){}
    /// \brief 配置一塊事先設定好大小的記憶體
    /// \return 傳回配置到的記憶體，如果失敗時傳回NULL
    virtual void* Allocate() = 0;
    /// \brief 釋放記憶體
    /// \param pObjMemory 先前用 Allocate 配置到的記憶體    
    virtual void Release( void* pObjMemory ) = 0;
    /// \brief 取得在初始化後所收集到已使用過的記憶體清單
    /// \param usedList 傳回初始化後所收集到已使用過的記憶體清單
    virtual void GetUsedList( std::list<void*>& usedList ) = 0;
    /// \brief 取得目前還可以配置的數量
    /// \return 目前還可以配置的數量
    virtual int GetAvailableCount() const = 0;
    /// \brief 取得每個物件的大小
    /// \return 傳回每個物件的大小
    virtual int GetObjectSize() const = 0;
    /// \brief 取得物件總共數量
    /// \return 傳回物件總共數量
    virtual int GetObjectNumber() const = 0;
};

/// \brief 用基本的 Heap 區塊來管理的配置管理器
class HeapMemoryAllocator : public MemoryAllocator {
public:
    HeapMemoryAllocator( int objSize, int objNum );
    HeapMemoryAllocator( const HeapMemoryAllocator& orig ) = delete;
    virtual ~HeapMemoryAllocator();
    
    // override from MemoryAllocator
    virtual void* Allocate();
    virtual void Release( void* pObjMemory );
    virtual void GetUsedList( std::list<void*>& usedList );
    virtual int GetAvailableCount() const;
    virtual int GetObjectSize() const{ return ObjSize_; }
    virtual int GetObjectNumber() const { return ObjNum_; }
    // end
private:
    int         ObjSize_;
    int         ObjNum_;
    void*       Pool_;
    
    mutable std::mutex      Lock_;
    std::queue<void*>       AvailableQueue_;
};

#if defined(_OS_LINUX_)
/// \brief 用 MemoryMapFile 配置及管理的配置管理器
class MMapMemoryAllocator : public MemoryAllocator {
public:
    MMapMemoryAllocator(const std::string& filename, int objSize, int objNum, bool isReuse);
    virtual ~MMapMemoryAllocator();
    
    // override from MemoryAllocator
    virtual void* Allocate();
    virtual void Release( void* pObjMemory );
    virtual void GetUsedList( std::list<void*>& usedList );
    virtual int GetAvailableCount() const;
    virtual int GetObjectSize() const{ return ObjSize_; }
    virtual int GetObjectNumber() const { return ObjNum_; }
    // end
private:
    std::string Filename_;                              //!< 檔案名稱
    int         ObjSize_;                               //!< 每塊要配置的記憶體大小
    int         ObjNum_;                                //!< 總數
    bool        IsReuse_;                               //!< 設定是否重用已Release的資料
    int         FileID_;                                //!< 檔案代碼
    char*       MMapFile_;                              //!< memory map 對應的記憶體位址
    
    mutable std::mutex      Lock_;
    std::queue<char*>       AvailableQueue_;        //!< 可用於配置使用的記憶體區塊
    std::list<char*>        UsedList_;              //!< 儲存剛載入時已被使用過的清單
    
private:
    /// \brief 檢查檔案是否存在及是否有值
    static bool IsFileExist( const std::string& filename );
};
#else //_WIN32
// FIXME : 暫時用，需配合Windows版修改
/// \brief 用 MemoryMapFile 配置及管理的配置管理器
class MMapMemoryAllocator : public MemoryAllocator {
public:
    MMapMemoryAllocator(const std::string& filename, int objSize, int objNum, bool isReuse);
    virtual ~MMapMemoryAllocator();
    
    // override from MemoryAllocator
    virtual void* Allocate();
    virtual void Release( void* pObjMemory );
    virtual void GetUsedList( std::list<void*>& usedList );
    virtual int GetAvailableCount() const;
    virtual int GetObjectSize() const{ return ObjSize_; }
    virtual int GetObjectNumber() const { return ObjNum_; }
    // end
private:
    std::string Filename_;                              //!< 檔案名稱
    int         ObjSize_;                               //!< 每塊要配置的記憶體大小
    int         ObjNum_;                                //!< 總數
    bool        IsReuse_;                               //!< 設定是否重用已Release的資料
    int         FileID_;                                //!< 檔案代碼
    char*       MMapFile_;                              //!< memory map 對應的記憶體位址
    
    mutable std::mutex      Lock_;
    std::queue<char*>       AvailableQueue_;        //!< 可用於配置使用的記憶體區塊
    std::list<char*>        UsedList_;              //!< 儲存剛載入時已被使用過的清單
    
private:
    /// \brief 檢查檔案是否存在及是否有值
    static bool IsFileExist( const std::string& filename );
};
#endif

template<typename T>
class MemoryAllocatorHandler{
public:
    MemoryAllocatorHandler() : Allocator_(NULL), Object_(NULL), IsRelease_(false){}
    MemoryAllocatorHandler(MemoryAllocator* ma) : Allocator_(ma), IsRelease_(false){
        if( ma->GetObjectSize() < sizeof(T) ){
            throw std::runtime_error( "MemoryAllocatorHanler object size not matched" );
        }
        
        void* pNewObject = ma->Allocate();
        if( pNewObject != NULL )
            Object_ = new(pNewObject) T;
        else
            Object_ = NULL;
    }
    MemoryAllocatorHandler(const MemoryAllocatorHandler& orig ) : Allocator_(NULL), Object_(NULL), IsRelease_(false) {
        *this = orig;
    }
    MemoryAllocatorHandler( MemoryAllocatorHandler&& orig ) : Allocator_(NULL), Object_(NULL), IsRelease_(false) {
        *this = std::move( orig );
    }
    ~MemoryAllocatorHandler(){
        if( Allocator_ && IsRelease_ ){
            Allocator_->Release( Object_ ); 
        }
    }
    MemoryAllocatorHandler& operator=( const MemoryAllocatorHandler& rhs ){
        if( this != &rhs ){
            Allocator_ = rhs.Allocator_;
            Object_ = rhs.Object_;
            IsRelease_ = rhs.IsRelease_;
        }
        return *this;
    }
    MemoryAllocatorHandler& operator=( MemoryAllocatorHandler&& rhs ){
        if( this != &rhs ){
            Allocator_ = rhs.Allocator_;
            Object_ = rhs.Object_;
            IsRelease_ = rhs.IsRelease_;

            rhs.Object_ = NULL;
            rhs.Allocator_ = NULL;
            rhs.IsRelease_ = false;
        }
        return *this;
    }
    T* operator->(){
        return Object_;
    }
    const T* const operator->() const{
        return Object_;
    }
    bool empty() const {
        return Object_ == NULL;
    }
    void Swap( MemoryAllocatorHandler&& rhs ){
        if( this != &rhs ){
            MemoryAllocatorHandler tmp( *this );
            Allocator_ = rhs.Allocator_;
            Object_ = rhs.Object_;
            IsRelease_ = rhs.IsRelease_;

            rhs.Object_ = tmp.Object_;
            rhs.Allocator_ = tmp.Allocator_;
            rhs.IsRelease_ = tmp.IsRelease_;
            
            tmp.Object_ = NULL;
            tmp.Allocator_ = NULL;
            tmp.IsRelease_ = false;
        }
    }
    
    void SetIsRelease( bool isRelease ){ IsRelease_ = isRelease; }
    bool GetIsRelease() const{ return IsRelease_; }
private:
    bool                IsRelease_;
    T*                  Object_;
    MemoryAllocator*    Allocator_;        
};

}

#endif /* UTILITY_MEMORYALLOCATOR_H */

