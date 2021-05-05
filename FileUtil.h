/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FileUtil.h
 * Author: Justin Lin
 *
 * Created on 2017年2月15日, 下午 5:55
 */

#ifndef UTILITY_FILEUTIL_H
#define UTILITY_FILEUTIL_H

#include <string>

namespace ProgLab{ namespace Utility{ 

/// \brief 收集一些有關檔案操作的工具類別
class FileUtil {
public:
    /// \brief 由指定的路徑及檔案樣式中，找出第一個符合樣式的檔名
    /// \param filePathPattern 要找尋的路徑及檔案樣式 ex /opt/test/Data_ 表示要在/opt/test/目錄中找到以Data_開頭的第一個檔案
    /// \return 傳回所找到的檔案的完整路徑
    static std::string FindFirstFileByPattern( const std::string& filePathPattern );
    /// \brief 將路徑分解出路徑及檔名的部份
    /// \param path 傳入要分解的字串
    /// \param rootPath 傳回路徑的部份，包含結尾的 / 
    /// \param filename 傳回檔名的部份
    /// \return 是否可以成功分解
    static bool ParsePath( const std::string& path, std::string& rootPath, std::string& filename );

    /// \brief 確認指定的檔案是否已存在
    /// \param filename 要查詢的檔案名稱(必要的話，要含路徑)
    /// \return 是否存在
    static bool IsFileExist( const std::string& filename );  
private:
    FileUtil();
    FileUtil(const FileUtil& orig);
    virtual ~FileUtil();
private:

};

}

#endif /* UTILITY_FILEUTIL_H */

