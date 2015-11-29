#include "Sender.h"


Sender::Sender() {

}


void Sender::init(std::string host, std::string port) {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    // set receiver info
    if( getaddrinfo(host.c_str(), port.c_str(), &hints, &receiverinfo) != 0 ){
        perror("sender getaddrinfo(): ");
        exit(1);
    }

    if( (senderFD = socket(receiverinfo->ai_family, receiverinfo->ai_socktype, receiverinfo->ai_protocol)) == -1){
        perror("sender socket(): ");
        exit(1);
    }
}


void Sender::mySend(struct update_msg * msg) {
    int len = (msg->addr).length() + 4;

    // std::cout << "send len: " << len << std::endl;

    char temp[len];
    memcpy(temp, msg, len);

    if( sendto(senderFD, temp, len, 0, receiverinfo->ai_addr, receiverinfo->ai_addrlen) == -1){
        perror("Sender sendto(): ");
        exit(1);
    }
}
