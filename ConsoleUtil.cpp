/* 
 * File:   ConsoleUtil.cpp
 * Author: 0007989
 * 
 * Created on 2014年3月21日, 上午 9:14
 */

#include "ConsoleUtil.h"
#include "CommonException.h"
#include <string.h>
#include <stdio.h>

namespace KGI_TW_Der_Utility
{   

ConsoleUtil::ConsoleUtil() {
}

ConsoleUtil::ConsoleUtil(const ConsoleUtil& orig) {
}

ConsoleUtil::~ConsoleUtil() {
}

int ConsoleUtil::WaitCommand( char *pCommand, int cmdSize )
{
    ::memset( pCommand, 0, cmdSize );

    int idx = 0;
    while(true)
    {
        
        try
        {
            char c = getchar();
            
            if( c != 0x0a )
            {
                    pCommand[idx] = c;
                    idx++;
                    if( idx >= cmdSize )
                            break;
            }
            else
                    break;
        }
        catch( CommonException &exp )
        {
            printf( "%d,%s\r\n", exp.GetLineNum(), exp.GetSource().c_str() );
        }
        catch( char const *pExp )
        {
            printf( "%s\r\n", pExp );
        }  
        catch(...)
        {
            printf( "Exception\r\n");
        }

    }
    return idx;
}

}