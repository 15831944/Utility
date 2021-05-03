/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MailNotifier.cpp
 * Author: Justin Lin
 * 
 * Created on 2017年5月10日, 下午 5:49
 */

#include "MailNotifier.h"

#include <stdexcept>

namespace KGI_TW_Der_Utility{ 

MailNotifier::MailNotifier() {
}

//MailNotifier::MailNotifier(const MailNotifier& orig) {
//}

MailNotifier::~MailNotifier() {
}

void MailNotifier::SendMessage( const MailMessage& msg ){
#if defined(_OS_LINUX_)
    FILE *mailpipe = popen("/usr/lib/sendmail -t", "w");
    if (mailpipe != NULL) {
        fprintf(mailpipe, "To: %s\n", msg.To.c_str());
        fprintf(mailpipe, "From: %s\n", msg.From.c_str());
        fprintf(mailpipe, "Subject: %s\n\n", msg.Subject.c_str() );
        fwrite(msg.Message.c_str(), 1, msg.Message.size(), mailpipe);
        fwrite(".\n", 1, 2, mailpipe);
        pclose(mailpipe);        
    }else {
        throw std::runtime_error("Failed to invoke sendmail");
    }    
#else	// _WIN32
	// TODO 
#endif
}

}