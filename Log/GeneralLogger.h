/* 
 * File:   GeneralLogger.h
 * Author: 0007989
 *
 * Created on 2015年1月26日, 上午 9:39
 */

#ifndef UTILITY_GENERALLOGGER_H
#define	UTILITY_GENERALLOGGER_H

#include "../Persistence/AbstractFilePersistence.h"
#include <string>
#include "../MacroRecipe.h"
#include <memory>
#include <stdio.h>
#include "../Log/ILogger.h"

namespace ProgLab{ namespace Utility{

    /// \brief 配合 LogSystem ，讓AbstractFilePersistence 可以支援 LogSystem 的wrapper類別
    /// 將產生好的 AbstractFilePersistence 指標交給 GeneralLogger 來保管，當GeneralLogger結束時，會自動呼叫 AbstractFilePersistence::Close() 函式
class GeneralLogger : public ILogger{
public:
    GeneralLogger( AbstractFilePersistence* pPersistenter );
    GeneralLogger( const GeneralLogger& orig );
    ~GeneralLogger();
    
    GeneralLogger& operator=( const GeneralLogger& orig );
    
    bool IsSameInstance( const GeneralLogger& rhs )const;
    
    PROP_GETSET_STR(std::string, LoggerName, m_LoggerName );
    int Write( const char* pBuffer, int length );
    
    // override from GeneralLogger
    virtual void WriteLogString( const char* pMsg );
    // end
protected:    
    std::shared_ptr<AbstractFilePersistence> m_pPersistenter;
    std::string m_LoggerName;
    
    void PersistenterDeletor( AbstractFilePersistence* pPersistenter){
        printf("PersistenterDeletor()\r\n");
                
        if( pPersistenter != NULL ){
            pPersistenter->Close();
            delete pPersistenter;                
        }
    }
};

/// \brief 讓LogSystem可以使用新的AbstractFilePersistence物件來記錄Log資訊
class GeneralLoggerWrapper : public ILogger{
public:
    GeneralLoggerWrapper(const GeneralLogger& logger ):m_Logger(logger){}
    
    virtual void WriteLogString( const char* pMsg );
        
private:
    GeneralLogger m_Logger;
};

}
#endif	/* UTILITY_GENERALLOGGER_H */

