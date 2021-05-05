/* 
 * File:   LogMessage.h
 * Author: 0007989
 *
 * Created on 2014年3月11日, 上午 11:33
 */

#ifndef UTILITY_LOGMESSAGE_H
#define	UTILITY_LOGMESSAGE_H

namespace ProgLab{ namespace Utility{      

/// Tool for manage the string buffer of log
///    
class LogMessage {
public:    
    LogMessage( int length = 100 );
    LogMessage(const LogMessage& orig);
    virtual ~LogMessage();
    
    int Format( const char *pFmt, ... );
    
    int GetLength() const{ return m_Length; }
    void SetLength( int length );	

    const char *GetMessage(){ return GetMessageImpl(); }

	/*
#ifdef WIN32
	const char *GetMessageW(){ return GetMessageImpl(); }
	const char *GetMessageA(){ return GetMessageImpl(); }
#else
	const char *GetMessage(){ return GetMessageImpl(); }
#endif
	*/
	

    const char *GetMessageImpl();
private:
    int m_Length;               //!< the length of message
    char *m_pMessage;           //!< the message buffer

};

}

#endif	/* UTILITY_LOGMESSAGE_H */

