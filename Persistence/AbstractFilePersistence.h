/* 
 * File:   AbstractFilePersistence.h
 * Author: 0007989
 *
 * Created on 2015年1月26日, 上午 8:13
 */

#ifndef UTILITY_ABSTRACTFILEPERSISTENCE_H
#define	UTILITY_ABSTRACTFILEPERSISTENCE_H

#include <string>

namespace KGI_TW_Der_Utility{ 

/// \brief 檔案讀寫的基底類別
class AbstractFilePersistence{
public:
    static const int MBSize = 1024 * 1024;          //!< 定義每個MB的位元組數
public:
    virtual ~AbstractFilePersistence(){};
    
    /// \brief 開啟檔案
    /// \param pFilename 檔案名稱
    /// \retnrn 成功與否
    virtual bool Open( const char* pFilename ) = 0;
    /// \brief 關閉檔案
    virtual void Close() = 0;
    /// \brief 寫入資料
    /// \param pBuffer 要被寫入的資料的buffer
    /// \param length 資料長度
    /// \return 實際寫入的長度
    virtual int Write( const char* pBuffer, int length ) = 0;
    /// \brief 讀取資料
    /// \param pBuffer 要讀取資料用的buffer
    /// \param length 儲存資料的buffer區的大小
    /// \return 實際讀取資料的長度
    virtual int Read( char* pBuffer, int length ) = 0;
    
    /// \brief 取得目前檔案的旳大小
    /// \return 目前檔案的大小
    virtual int GetSize() const = 0;
    
    const std::string& GetFilename() const { return m_Filename; }
    
    /// \brief 將給定的路徑分割出檔案名稱及路徑名稱
    /// \param path 要分割的原始路徑
    /// \param rootPath 傳回路徑名稱
    /// \param filename 傳回檔案名稱
    /// \return 成功與否
    static bool ParsePath( const std::string& path, std::string& rootPath, std::string& filename );    
    /// \brief 取得在指定目錄中及檔案名稱中的最後一個索引值
    /// 檔案的格式為 /path/filename.ext.index
    /// \param filePath 要查詢的路徑及檔案名稱
    /// \return 傳回最後一個索引值
    static int GetLastIndex(const std::string& filePath );
    
protected:
    std::string m_Filename;
};

}
#endif	/* UTILITY_ABSTRACTFILEPERSISTENCE_H */

