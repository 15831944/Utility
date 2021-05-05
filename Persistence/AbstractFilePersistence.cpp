/* 
 * File:   AbstractFilePersistence.cpp
 * Author: 0007989
 * 
 * Created on 2015年1月26日, 上午 8:13
 */

#include "AbstractFilePersistence.h"

#include <assert.h>
#ifdef _OS_LINUX_
#include <dirent.h>
#else // _WIN32
#include <Windows.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <cstdio>

namespace ProgLab{ namespace Utility{ 

#ifdef _OS_LINUX_
int AbstractFilePersistence::GetLastIndex( const std::string& filePath ){
    DIR* d;
    struct dirent* dir;
    int lastIndex = 0;
    
    std::string rootPath, filename;
    if( ParsePath( filePath, rootPath, filename ) == false )
        return 0;
    if( filename.size() <= 0 )
        return 0;
    
    filename += '.';
    
    if( rootPath.size() <= 0 ){
        rootPath = ".";
    }        
    
    d = opendir( rootPath.c_str()); 
    if( d != NULL )
    {    
        while( (dir = readdir(d)) != NULL )
        {
            std::string fileInPath = dir->d_name;
            if( strstr( dir->d_name, filename.c_str() )  != NULL  &&
                    fileInPath.size() > filename.size() )
            {
                std::string postFix( fileInPath.c_str(), filename.size(), fileInPath.size() - filename.size() );
                int crtIndex = atoi( postFix.c_str() );
                if( crtIndex > lastIndex )
                    lastIndex = crtIndex;
            }
        }
        closedir(d);
        return lastIndex;
    }
    else
        return 0;        
}
#else
int AbstractFilePersistence::GetLastIndex(const std::string& filePath){
	
	std::string rootPath, filename;
	if (ParsePath(filePath, rootPath, filename) == false)
		return 0;
	if (filename.size() <= 0)
		return 0;

	filename += '*';
        
        if( rootPath.size() <= 0 ){
            rootPath = ".";
        } 
        
        int lastIndex = 0;

	std::string searchPath = rootPath + "\\" + filename;

	WIN32_FIND_DATA findFile;
	HANDLE hFind = FindFirstFile((LPCSTR)searchPath.c_str(), &findFile);
	while (hFind != INVALID_HANDLE_VALUE){
		std::string foundName( (const char*)findFile.cFileName);
		std::size_t pos = foundName.rfind(".");
		if (pos != std::string::npos){
			std::string indexValue = foundName.substr(pos, foundName.size() - pos);
			int crtIndex = atoi(indexValue.c_str());
			if (crtIndex > lastIndex)
                            lastIndex = crtIndex;
		}
		FindNextFile(hFind, &findFile);
	}
	return lastIndex;
}
#endif

bool AbstractFilePersistence::ParsePath( const std::string& path, std::string& rootPath, std::string& filename ){
#ifdef _OS_LINUX_
    std::string::size_type splitPos = path.rfind( '/' );
#else // _WIN32
	std::string::size_type splitPos = path.rfind( '\\' );
#endif
	if( splitPos == std::string::npos )
	{
		rootPath = "";
		filename = path;
	}
	else
	{
		rootPath.assign(path.c_str(), splitPos + 1);
		int filenameLength = path.size() - splitPos - 1;
		filename.assign(path.c_str(), splitPos + 1, filenameLength);
	}
    return true;
}



}