/* 
 * File:   ConsoleLogger.cpp
 * Author: 0007989
 * 
 * Created on 2014年1月27日, 下午 2:46
 */

#include "ConsoleLogger.h"
#include "LogSystem.h"

#include <iostream>
#include <stdio.h>

namespace KGI_TW_Der_Utility
{   

ConsoleLogger::ConsoleLogger() {
}

ConsoleLogger::ConsoleLogger( const char *pName ){
    m_LoggerName = pName;
}

ConsoleLogger::~ConsoleLogger() {
}

void ConsoleLogger::WriteLogString( const char* pMsg ){
    try
    {
        std::unique_lock<std::mutex> al(m_Lock);
        
        std::cout << pMsg;                

    }
    catch( ... )
    {     
    }
}
//
//void ConsoleLogger::WriteLog( LogLevel logLevel, const std::string& msg ){    
//    
//    if( logLevel >= m_MaxLogLevel )
//    {        
//        WriteLog( msg.c_str() );
//    }    
//}

}
