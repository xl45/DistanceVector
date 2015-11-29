#ifndef RECEIVER_H
#define RECEIVER_H

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
#include "Sender.h"


class Receiver {
    public:
        Receiver(std::string port);
        virtual ~Receiver();
        void myRecv();

    private:
        struct addrinfo hints;
        struct addrinfo *receiverinfo;
        struct sockaddr_storage sender_addr;
        socklen_t addr_len;
        int receiverFD;
};


#endif // RECEIVER_H
