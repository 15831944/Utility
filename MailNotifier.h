/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MailNotifier.h
 * Author: Justin Lin
 *
 * Created on 2017年5月10日, 下午 5:49
 */

#ifndef UTILITY_MAILNOTIFIER_H
#define UTILITY_MAILNOTIFIER_H

#include <string>

namespace KGI_TW_Der_Utility{ 

class MailNotifier {
public:
    struct MailMessage{
        std::string From;
        std::string To;
        std::string Subject;
        std::string Message;
    };
public:
    MailNotifier();
    MailNotifier(const MailNotifier& orig) = delete;
    virtual ~MailNotifier();
    
    static void SendMessage( const MailMessage& msg );
private:

};

}

#endif /* UTILITY_MAILNOTIFIER_H */

