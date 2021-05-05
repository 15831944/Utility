/* 
 * File:   SocketException.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年7月28日, 上午 8:50
 */

#ifndef UTILITY_SOCKETEXCEPTION_H
#define	UTILITY_SOCKETEXCEPTION_H

#include <string>
#include <stdexcept>

namespace ProgLab{ namespace Utility{

class SocketException : public std::exception {
public:
    SocketException() throw() : std::exception(){}
    SocketException( const char *pMsg );
    SocketException(const SocketException& orig);
    virtual ~SocketException() throw();
    
    SocketException& operator=( const SocketException& orig );
        
    virtual const char* what() const throw() { return m_Message.c_str(); }
private:
    std::string m_Message;
};

class SocketWriteException : public SocketException{
public:
    SocketWriteException( int willSentBytes, int sentBytes, const char* pMsg );
    SocketWriteException( const SocketWriteException& orig );
    virtual ~SocketWriteException() throw(){};
    
    SocketWriteException& operator=( const SocketWriteException& orig );
    
    int GetWillSentBytes() const { return m_WillSentBytes; }
    int GetSentBytes() const { return m_SentBytes; }
private:
    int m_WillSentBytes, m_SentBytes;
};


}
#endif	/* UTILITY_SOCKETEXCEPTION_H */

