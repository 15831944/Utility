/* 
 * File:   HashKeyHelper.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2016年8月17日, 下午 12:02
 */

#ifndef UTILITY_HASHKEYHELPER_H
#define	UTILITY_HASHKEYHELPER_H

#include <cstddef>

namespace ProgLab{ namespace Utility{   

    /// \brief 實作 FNV-1 的 hash 函式
    ///
    /// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    ///
class HashKeyHelper{
    public:        
        static const std::size_t m_Prime = 167771619;
        static const std::size_t m_Offset = 2166136261;
        
        HashKeyHelper() : m_HashValue(m_Offset){}
        HashKeyHelper( const HashKeyHelper& orig ){ 
            *this = orig;
        }
        
        HashKeyHelper& operator=( const HashKeyHelper& orig ){
            if( this != &orig ){
                m_HashValue = orig.m_HashValue;
            }
            return *this;
        }
        
        void Reset(){ m_HashValue = m_Offset; }
        
        std::size_t Append( std::size_t hashValue ){
            m_HashValue *= m_Prime;
            m_HashValue ^= hashValue;
            return m_HashValue;
        }
        
        std::size_t GetHashValue() const{ return m_HashValue; }
        
    private:
        std::size_t m_HashValue;
};

}
#endif	/* UTILITY_HASHKEYHELPER_H */

