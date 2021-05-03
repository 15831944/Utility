/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CharString.h
 * Author: Justin Lin
 *
 * Created on 2017年10月2日, 下午 2:55
 */

#ifndef UTILITY_CHARSTRING_H
#define UTILITY_CHARSTRING_H

#include <cstddef>
#include <stdexcept>
#include <string>

#include "CommonDef.h"

BEGIN_NAMESPACE( KGI_TW_Der_Utility )

/// \brief 使用Local空間的字串
///
///
template <int TSize>
class CharString{
public:
    static const std::size_t  StringSize;
    
public:
    CharString(){
        Buffer_[0] = 0x00;        
    }
    explicit CharString( const std::string& src ){
        std::size_t newLength = std::min( StringSize, src.length() );
        memcpy( Buffer_, src.c_str(), newLength );
        Buffer_[newLength] = 0x00;        
    }
    CharString( const char* pSrc, std::size_t length ){
        std::size_t newLength = std::min( StringSize, length );
        memcpy( Buffer_, pSrc, newLength );
        Buffer_[newLength] = 0x00;        
    }
    CharString( const CharString& orig ){
        //static_assert( orig.StringSize != StringSize );
        memcpy( Buffer_, orig.Buffer_, orig.Length_ );             
    }
    
    CharString& operator=( const std::string& rhs ){
        std::size_t newLength = std::min( StringSize, rhs.length() );
        memcpy( Buffer_, rhs.c_str(), newLength );
        Buffer_[newLength] = 0x00;        
    }
    
    /// \brief 取得所有可用空間
    /// \return 所有可用空間
    std::size_t GetCapacity() const{ return StringSize; }
    /// \brief 取得目前字串長度
    /// \return 目前字串長度
    std::size_t Size() const {
        return strlen( Buffer_ );
    }
    /// \brief 取得字串資料
    /// \return 字串資料
    const char* Data() const { return Buffer_; }
    /// \brief 取得字串資料
    /// \return 字串資料
    char* Data() { return Buffer_; }
    char& operator[]( int index ){
        if( index >= 0 && index < StringSize )
            return Buffer_[index];
        else
            throw std::runtime_error( "Index out of range" );
    }
    const char& operator[]( int index) const{
        if( index >= 0 && index < StringSize )
            return Buffer_[index];
        else
            throw std::runtime_error( "Index out of range" );
    }
    
    template <typename T, typename ... Args>
    std::size_t Format(T const * const format,
                Args const & ... args)
    {
        int const result = snprintf(Buffer_,
                                StringSize+1,
                                format,
                                Argument(args) ...);        
        return result;
    }
private:
    char            Buffer_[TSize+1];       //!< 字串資料    
        
};

template<int TSize>
const std::size_t CharString<TSize>::StringSize = TSize;

END_NAMESPACE( KGI_TW_Der_Utility )
#endif /* UTILITY_CHARSTRING_H */

