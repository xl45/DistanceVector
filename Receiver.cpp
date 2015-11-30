#include "Receiver.h"


Receiver::Receiver(std::string port) {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if( getaddrinfo("0.0.0.0", port.c_str(), &hints, &receiverinfo) != 0 ){
        perror("receiver getaddrinfo(): ");
        exit(1);
    }

    if( (receiverFD = socket(receiverinfo->ai_family, receiverinfo->ai_socktype, receiverinfo->ai_protocol)) == -1){
        perror("receiver socket(): ");
        exit(1);
    }

    if( bind(receiverFD, receiverinfo->ai_addr, receiverinfo->ai_addrlen) == -1 ){
        perror("receiver bind(): ");
        exit(1);
    }
}


void Receiver::myRecv() {
    while(true) {
        char buffer[5];
        addr_len = sizeof(sender_addr);

        int n = recvfrom(receiverFD, buffer, 5, 0, (struct sockaddr *)&sender_addr, &addr_len);

        if(n < 0) {
            perror("receiver recvfrom(): ");
            continue;
        }

        struct update_msg msg;
        memcpy(&msg, buffer, 5);

        std::cout << "\nrecv following msg: \n" << msg.addr 
        << ", " << msg.cost << std::endl;

        // update corresponding ttl
        // see if there is update
        // if there is, update table and send update msg
        // deduct ttl in unreceived routing entry, if there is 0, timeout and set to infinity(send this ?) 
    }
}


Receiver::~Receiver() {
    //dtor
}
