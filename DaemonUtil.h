/* 
 * File:   DaemonUtil.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年3月6日, 上午 11:52
 */

#ifndef UTILITY_DAEMONUTIL_H
#define	UTILITY_DAEMONUTIL_H

namespace KGI_TW_Der_Utility{  

/// \brief 讓程式以Daemon方式執行的工具類別
class DaemonUtil {
public:
    /// 定義 signal hadler 的callback函式型別
    typedef  void (*CustomerSignalHandler)( int sig );
public:    
    DaemonUtil(const DaemonUtil& orig) = delete;    
    
    /// \brief 將目前程序的pid儲存到指定的檔案中
    /// \param pPIDFile 要儲存目前程式的pid的檔案名稱
    static void SavePIDToFile( const char *pPIDFile );
    /// \brief 將目前程序設定成適合daemon執行的方式<br>
    /// 使用這種daemon的話，要用以下的方式執行<br>
    /// nohup ./speedygateway -p 20014 -i 10.3.99.142 -c ./cfg/speedygateway.ini -d 0 -M 200 -D . --e2e debug 1>/dev/null 2>&1 &
    /// \param pRunningDir 程序執行的主要目錄
    /// \param pLockFilename 用來鎖定以避免多次執行的檔案名稱
    /// \param pHandler 設定 signal 的處理函式
    /// \param 傳回成功與否
    static bool Daemonize( const char* pRunningDir, const char* pLockFilename, CustomerSignalHandler pHanlder  );
    /// \brief 將訊息寫入指定檔案中
    /// \param pLogFile 檔案名稱
    /// \param pMsg 要寫入的訊息字串
    static void LogToFile( const char* pLogFile, const char* pMsg );
private:
    DaemonUtil();
    virtual ~DaemonUtil();

};

}

#endif	/* UTILITY_DAEMONUTIL_H */

