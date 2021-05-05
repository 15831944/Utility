/* 
 * File:   PersistenceLineReader.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2016年7月20日, 下午 5:22
 */

#ifndef UTILITY_PERSISTENCELINEREADER_H
#define	UTILITY_PERSISTENCELINEREADER_H

#include "AbstractFilePersistence.h"

#include <functional>
#include <string>

namespace ProgLab{ namespace Utility{ 

/// \brief 配合 AbstractFilePersistence 讀取一行行的資料
/// 斷行字元預設為 \r\n
class PersistenceLineReader {
public:
    
    virtual ~PersistenceLineReader();
    
    /// \brief 讀取一行行的資料。
    /// \param pPersistence 要用來讀取資料的串流
    static void Read( AbstractFilePersistence* pPersistence, std::function<void(const std::string&)> lineFeed );
private:
    PersistenceLineReader();
    PersistenceLineReader(const PersistenceLineReader& orig);

};

}
#endif	/* UTILITY_PERSISTENCELINEREADER_H */

