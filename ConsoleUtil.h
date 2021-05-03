/* 
 * File:   ConsoleUtil.h
 * Author: 0007989
 *
 * Created on 2014年3月21日, 上午 9:14
 */

#ifndef UTILITY_CONSOLEUTIL_H
#define	UTILITY_CONSOLEUTIL_H

namespace KGI_TW_Der_Utility
{   

class ConsoleUtil {
private:
    ConsoleUtil();
    ConsoleUtil(const ConsoleUtil& orig);
    virtual ~ConsoleUtil();
public:    
    /// \brief 等待使用者由命令列視窗中輸入字串
    /// \param pCommand 接收輸入的字串的buffer
    /// \param cmdSize buffer大小
    /// \return 傳回此次接收到的字串長度
    static int WaitCommand( char *pCommand, int cmdSize );
private:

};

}

#endif	/* UTILITY_CONSOLEUTIL_H */

