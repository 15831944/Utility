/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   UtilityVersion.h
 * Author: Justin Lin
 *
 * Created on 2017年2月23日, 下午 6:27
 */

#ifndef UTILITY_UTILITYVERSION_H
#define UTILITY_UTILITYVERSION_H

#include <string>

// 方便其它C++程式可用 #if 的方式來處理
#define UTILITY_VER_MAIN       1
#define UTILITY_VER_MAJOR      11
#define UTILITY_VER_MINER      4

namespace ProgLab{ namespace Utility{

/// \brief 取得 Utility 版本資訊
class UtilityVersion {
public:
    /// \brief 取得主要版本號
    /// \return 主要版本號
    static unsigned int GetMainVersion(){ return UTILITY_VER_MAIN; }
    /// \brief 取得次要版本號
    /// \return 次要版本號
    static unsigned int GatMajorVersion(){ return UTILITY_VER_MAJOR; }
    /// \brief 取得修正版本號
    /// \return 修正版本號
    static unsigned int GetMinerVersion(){ return UTILITY_VER_MINER; }
    /// \brief 取得版本資訊
    /// \return 傳回版本資訊字串型式，例如 "1.0.0"
    static std::string GetVersion(){
        return std::string( "1.11.4" );
    }
    /// \brief 詢問此版本是否支援某項功能，功能是用字串型式來表示的
    /// \param feature 功能
    /// \return 是否支援
    static bool IsSupportFeature( const std::string& feature ){
        // 後續再實作
        return false;
    }
    /// \brief 取得目前的分支資訊
    /// \return 目前的分支資訊
    static std::string GetBranchInfo();
    static const std::string& GetBuildDate(){
        return BuildDate_;
    }
    static const std::string& GetBuildTime(){
        return BuildTime_;;
    }
private:
    UtilityVersion();
    UtilityVersion(const UtilityVersion& orig);
    virtual ~UtilityVersion();

private:
    static std::string BuildDate_;
    static std::string BuildTime_;
};

}

/*
 * 20190317 1.11.4
 * Add : BracesStructure add ParseV2() for enhancing the parsing latency.
 * 
 * 20190215 1.11.3
 * Fix : ConsolePersistence::Write() use char[] instead of char to prevent from valgrind detects 'Mismatched free()'
 * 
 * 20181228 1.11.2
 * Fix : Use owner_before to compare smart pointers in SubscribePool
 *
 * 20181227 1.11.1
 * New : Support AddIfNotExist in SubscribePool with customized comparator
 *
 * 20180831 1.11.0
 * New : 新增IntervalTrigger
 *
 * 20180608 1.10.0
 * New : 新增MMapSeqGenerator
 *
 *
 * 20180525 1.9.0
 * New : 不再編譯OpenCC
 *
 *
 * 20180328 1.8.2
 * Fix : 修正 SocketChannel 中 SetReadTimeout() 及 SetWriteTimeout() 時間計算錯誤問題
 *
 * 20180212 1.8.1
 * Fix : 修正SwitchBuffer，如果切換時間太短時，可能造成Exception的問題
 *
 * 20171117 1.8.0
 * * Add MemoryPool & MemoryPoolFixSize
 *
 * 20170517 1.3.0
 * * Add ScheduleNotifier
 */
#endif /* UTILITY_UTILITYVERSION_H */

