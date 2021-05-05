/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MMapSeqGenerator.h
 * Author: Justin Lin
 *
 * Created on 2018年6月8日, 下午 12:43
 */

#ifndef UTILITY_MMAPSEQGENERATOR_H
#define UTILITY_MMAPSEQGENERATOR_H

#if __cplusplus >= 201103L
#include <atomic>
#else
#include <cstdatomic>
#endif
#include <string>

namespace ProgLab{ namespace Utility{

/// \brief 利用Memory Map File方式記錄的序號產生器，可利用Memory Map File的自動保存功能
class MMapSeqGenerator{
public:
    /// \brief constructor
    /// \param file Memory map file要對應及保存的檔案名稱(含路徑)
    /// \param initValue 如果是第一次產生時，要初始化的設定值
    MMapSeqGenerator( const std::string& file, unsigned long initValue );
    MMapSeqGenerator( const MMapSeqGenerator& orig ) = delete;
    ~MMapSeqGenerator();
    
    unsigned long Increase();
    /// \brief 取回目前的序號
    /// \return 目前的序號
    unsigned long GetSequence() const;
    
    unsigned long operator++();
    unsigned long operator++(int);
    
private:
    int     FileID_;
    int     FileSize_;
    char*   pMMapFile_;
    
    std::atomic_ulong*  pSeqNumber_;
};

}

#endif /* UTILITY_MMAPSEQGENERATOR_H */

