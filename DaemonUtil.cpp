/* 
 * File:   DaemonUtil.cpp
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 * 
 * Created on 2015年3月6日, 上午 11:52
 */

#include "DaemonUtil.h"
#include <stdio.h>
#include <memory.h>
#ifdef _OS_LINUX_
#include <unistd.h>
#else
#endif
#include <signal.h>
#include <fcntl.h>
#include <string>
#include "DateTime.h"
#include "StringTemplateUtil.h"

namespace ProgLab{ namespace Utility{ 

DaemonUtil::DaemonUtil() {
}

//DaemonUtil::DaemonUtil(const DaemonUtil& orig) {
//}

DaemonUtil::~DaemonUtil() {
}
#ifdef _OS_LINUX_
void DaemonUtil::SavePIDToFile( const char *pPIDFile ){
    FILE *pFID = fopen( pPIDFile, "w+" );
    if( pFID != NULL )
    {
        char buffer[100];
        memset( buffer, 0, 100 );        
        int size = snprintf( buffer, 100, "%d", getpid() );
        fwrite( buffer, size, 1, pFID );
        fclose( pFID );
    }
}
#else
void DaemonUtil::SavePIDToFile(const char *pPIDFile) {

	FILE *pFID = NULL;
	errno_t errorCode = fopen_s(&pFID, pPIDFile, "w+");	

	if ( errorCode == 0)
	{
		char buffer[100];
		memset(buffer, 0, 100);
		int size = sprintf_s(buffer, 100,  "%d", GetCurrentProcessId());
		fwrite(buffer, size, 1, pFID);
		fclose(pFID);
	}
}
#endif
// 使用這種daemon的話，要用以下的方式執行
// nohup ./speedygateway -p 20014 -i 10.3.99.142 -c ./cfg/speedygateway.ini -d 0 -M 200 -D . --e2e debug 1>/dev/null 2>&1 &
bool DaemonUtil::Daemonize( const char* pRunningDir, const char* pLockFilename, CustomerSignalHandler pHanlder  )
{
#ifdef _OS_LINUX_
    int lfp;
    char str[10];    
    chdir(pRunningDir); /* change running directory */
    lfp=open(pLockFilename,O_RDWR|O_CREAT,0640);
    if (lfp<0) return false; /* can not open */
    if (lockf(lfp,F_TLOCK,0)<0) return false; /* can not lock */
    /* first instance continues */
    sprintf(str,"%d\n",getpid());
    write(lfp,str,strlen(str)); /* record pid to lockfile */
    signal(SIGCHLD,SIG_IGN); /* ignore child */
    signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP,pHanlder); /* catch hangup signal */
    signal(SIGTERM,pHanlder); /* catch kill signal */
    signal(SIGUSR1, pHanlder);
    signal(SIGUSR2, pHanlder);
#else
	return true;
#endif
    
    return true;
}

void DaemonUtil::LogToFile( const char* pLogFile, const char* pMsg ){
    DateTime now;
    std::string lm;
    StringTemplateUtil::Format( lm, "%04d-%02d-%02d %02d:%02d:%02d.%03d.%03d %s", now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond(), now.GetMillisecond(), now.GetMicrosecond(), pMsg );
#ifdef _OS_LINUX_
    FILE *logfile;
    logfile=fopen(pLogFile,"a");
#else
	FILE *logfile = NULL;
	errno_t errorCode = fopen_s(&logfile, pLogFile, "a");
#endif
    if(!logfile) return;
    fprintf(logfile,"%s\n",lm.c_str() );
    fclose(logfile);
}

}