/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FileUtil.cpp
 * Author: Justin Lin
 * 
 * Created on 2017年2月15日, 下午 5:55
 */

#ifdef _OS_LINUX_
#include <dirent.h>
#include <sys/stat.h>
#else // _WIN32
#include <Windows.h>
#endif
#include <string.h>

#include "FileUtil.h"

namespace ProgLab{ namespace Utility{ 

FileUtil::FileUtil() {
}

FileUtil::FileUtil(const FileUtil& orig) {
}

FileUtil::~FileUtil() {
}

bool FileUtil::ParsePath( const std::string& path, std::string& rootPath, std::string& filename ){
    if( path.empty() )
        return false;
    
#if defined(_OS_LINUX_)
    std::string::size_type splitPos = path.rfind( '/' );
#else // _WIN32
    std::string::size_type splitPos = path.rfind( '\\' );
#endif
    if( splitPos == std::string::npos ){
        rootPath = "";
        filename = path;
    }else{
        rootPath.assign(path.c_str(), splitPos + 1);
        int filenameLength = path.size() - splitPos - 1;
        filename.assign(path.c_str(), splitPos + 1, filenameLength);
    }
    return true;
}

#if defined( _OS_LINUX_ )
std::string FileUtil::FindFirstFileByPattern( const std::string& filePathPattern ){
    std::string result;
    
    DIR* d;
    struct dirent* dir;
    int lastIndex = 0;
    
    std::string rootPath, filename;
    if( ParsePath( filePathPattern, rootPath, filename ) == false )
        return result;
    if( filename.size() <= 0 )
        return result;
    
    //filename += '.';
    // 如果沒有指定路徑，就用目前的目錄
    if( rootPath.size() <= 0 ){
        rootPath = "./";
    }        
    
    d = opendir( rootPath.c_str()); 
    if( d != NULL ){    
        while( (dir = readdir(d)) != NULL ){
            std::string fileInPath = dir->d_name;
            if( strstr( dir->d_name, filename.c_str() )  != NULL  &&
                    fileInPath.size() > filename.size() ){
                result = dir->d_name;
            }
        }
        closedir(d);        
    }
    
    if( result.empty() == false ){
        // 再組回前面的路徑    
        result = rootPath + result;   
    }
    
    return result;
}

bool FileUtil::IsFileExist( const std::string& filename ){
    struct stat fileStat;
    int result = stat( filename.c_str(), & fileStat );
    if( result != 0 ){
        return false;
    }else{
        return fileStat.st_size > 0 ;
    }
}
#else // _WIN32
std::string FileUtil::FindFirstFileByPattern( const std::string& filePathPattern ){
    std::string result;  
    
    std::string rootPath, filename;
    if( ParsePath( filePathPattern, rootPath, filename ) == false )
        return result;
    if( filename.size() <= 0 )
        return result;
    
    //filename += '.';
    // 如果沒有指定路徑，就用目前的目錄
    if( rootPath.size() <= 0 ){
        rootPath = "./";
    }        
    
    std::string searchPath = rootPath + "\\" + filename;

    WIN32_FIND_DATA findFile;
    HANDLE hFind = FindFirstFile((LPCSTR)searchPath.c_str(), &findFile);
    while (hFind != INVALID_HANDLE_VALUE){
            std::string foundName( (const char*)findFile.cFileName);
            return foundName;
    }
    
    if( result.empty() == false ){
        // 再組回前面的路徑    
        result = rootPath + result;   
    }
    
    return result;
}

bool FileUtil::IsFileExist( const std::string& filename ){
    // TODO
    static_assert( false );
    return false;
}
#endif


}