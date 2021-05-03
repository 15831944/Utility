/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LocalString.h
 * Author: Justin Lin
 *
 * Created on 2017年6月19日, 下午 12:00
 */

#ifndef UTILITY_LOCALSTRING_H
#define UTILITY_LOCALSTRING_H

#include "CommonDef.h"

BEGIN_NAMESPACE( KGI_TW_Der_Utility )

/// \brief 類似 std::string 只是所使用的空間不是動態由 heap 中配置來的，而是直接在本地端配置的
/// 好處：
/// 減少執行 heap allocation 時間
/// 存取時不用再透過指標去遠端取得資料，減少 memory segment falt 的機會，減少 memory cache中找不到資料，要再重新載入字串所在位置區域的資料所花費的時間
template< int initSize >
class LocalString{
public:
    LocalString():Size_(initSize),Length_(0){
        static_assert( initSize > 0, "LocalString init size is zero" );
        memset( Data_, 0, sizeof(Data_) );        
    }
    LocalString(const LocalString& orig ):Size_(initSize),Length_(0){
        static_assert( initSize > 0, "LocalString init size is zero" );
        *this = orig;        
    }
    ~LocalString() = default;    
    
    LocalString& operator=( const LocalString& rhs ){
        if( this != &rhs ){
            if( Size_ < rhs.Size_ ){
                // error
                throw std::runtime_error( "LocalString assign operator Size is not enough" );
            }else{             
                Length_ = rhs.Length_;                
                memcpy( Data_, rhs.Data_, Length_ );
                Data_[Length_] = '\0';
            }
        }
        return *this;
    }
    
    int GetSize() const{ return Size_; }
    int GetLength() const{ return Length_; }
    void SetLength( int length ){ Length_ = length; }
    const char* c_str() const{ return Data_; }
    char& operator[]( int index ){
        if( index >= 0 && index < Size_ )
            return Data_[index];
        else
            throw std::runtime_error( "Index out of range" );
    }
    const char& operator[]( int index) const{
        if( index >= 0 && index < Size_ )
            return Data_[index];
        else
            throw std::runtime_error( "Index out of range" );
    }
    char* GetData() { return Data_; }
    
    template <typename T>
    static T Argument(T value) 
    {
        return value;
    }
    
    /// \brief 格式化字串，但如超過長度時會丟出exception
    template <typename ... Args>
    int Format( const char* pFmt, Args const & ... args  ){
        int needSize = snprintf( Data_, Size_ + 1, pFmt, Argument(args) ... );
        if( needSize > Size_ ){
            Length_ = 0;
            Data_[0] = '\0';
            throw std::runtime_error("out of range" );
        }else{
            Length_ = needSize;
            Data_[needSize] = '\0';            
        }   
    }
    
    bool empty() const{
        return Length_ == 0;        
    }
    
    bool operator==( const LocalString& rhs ) const{
        return strncmp( Data_, rhs.Data_, Length_ ) == 0;
    }
private:
    int     Size_;
    int     Length_;
    char    Data_[initSize+1];
};

END_NAMESPACE( KGI_TW_Der_Utility )

#endif /* UTILITY_LOCALSTRING_H */

