/* 
 * File:   GeneralLogger.cpp
 * Author: 0007989
 * 
 * Created on 2015年1月26日, 上午 9:39
 */

#include "GeneralLogger.h"
#include <functional>
#include <string.h>

namespace KGI_TW_Der_Utility{

// GeneralLogger
GeneralLogger::GeneralLogger( AbstractFilePersistence* pPersistenter )
    :m_pPersistenter(pPersistenter,std::bind(&GeneralLogger::PersistenterDeletor,this,std::placeholders::_1)){    
}

GeneralLogger::GeneralLogger( const GeneralLogger& orig ){
    *this = orig;
}

GeneralLogger::~GeneralLogger(){ 
    printf("~GeneralLogger()\r\n");
}

GeneralLogger& GeneralLogger::operator=( const GeneralLogger& orig ){
    if( this != &orig ){
        m_pPersistenter = orig.m_pPersistenter;
        m_LoggerName = orig.m_LoggerName;
    }
    return *this;
}

bool GeneralLogger::IsSameInstance( const GeneralLogger& rhs )const{
    return m_pPersistenter.get() == rhs.m_pPersistenter.get();
}

int GeneralLogger::Write( const char* pBuffer, int length ){
    if( m_pPersistenter != NULL ){
        return m_pPersistenter->Write( pBuffer, length );
    }
    else
        return 0;
}

void GeneralLogger::WriteLogString( const char* pMsg ){
    if( pMsg != NULL ){
        int length = strlen(pMsg);
        this->Write( pMsg, length );
    }
}

void GeneralLoggerWrapper::WriteLogString( const char* pMsg ){
    if( pMsg != NULL ){
        int length = strlen(pMsg);
        m_Logger.Write( pMsg, length );
    }
}

}