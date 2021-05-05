/* 
 * File:   CharArrayPool.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年8月5日, 下午 1:42
 */

#ifndef UTILITY_CHARARRAYPOOL_H
#define	UTILITY_CHARARRAYPOOL_H

namespace ProgLab{ namespace Utility{

class CharArrayPool {
public:
    CharArrayPool(int count, int lengthPerItem);
    CharArrayPool(const CharArrayPool& orig) = delete;
    virtual ~CharArrayPool();
    
    const char* const * GetItems() const { return m_pItems; }
    void Clear();
    void Copy( int itemIndex, const char* pSrc, int length );
private:
    int m_Count, m_LengthPerItem;
    char* m_pPool;
    char** m_pItems;

};

}
#endif	/* UTILITY_CHARARRAYPOOL_H */

