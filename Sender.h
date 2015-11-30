#ifndef SENDER_H_INCLUDED
#define SENDER_H_INCLUDED

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "common.h"


class Sender {
    public:
        Sender();
        void init(std::string host, std::string port);
        void mySend(struct update_msg * msg);

    private:
        struct addrinfo hints;
        struct addrinfo *receiverinfo;
        int senderFD;
};


#endif // SENDER_H_INCLUDED
