#include "Receiver.h"

extern route_entry routing_table[NODE_NUM];
extern int ttl;
extern STR host_names[NODE_NUM];
extern int graph[NODE_NUM][NODE_NUM];

void updateTTL(char node);
void bellman_ford();

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

        struct sockaddr_in *sin = (struct sockaddr_in *)&sender_addr;
        // unsigned char *ip = (unsigned char *)&sin->sin_addr.s_addr;
        char *ip = inet_ntoa(sin->sin_addr);
        std::string str_ip(ip);

        std::cout << "\nrecv following msg: \n" << msg.node 
        << ", " << msg.cost << " (ip: " << str_ip << ")";

        // update coressponding ttl
        char fromNode;
        if(str_ip == NODE_A_IP)
            fromNode = 's';
        else if(str_ip == NODE_B_IP)
            fromNode = 'b';
        else if(str_ip == NODE_C_IP)
            fromNode = 'c';
        else if(str_ip == NODE_D_IP)
            fromNode = 'd';
        updateTTL(fromNode);

        // update graph
        int index_from;
        int index_to;
        for(int i = 0; i < NODE_NUM; i++) {
            if(host_names[i].at(0) == fromNode) {
                index_from = i;
            }
            if(host_names[i].at(0) == msg.node) {
                index_to = i;
            }
        }
        graph[index_from][index_to] = msg.cost;
        
        // run bellman-ford to update routing table
        bellman_ford();
    }
}


Receiver::~Receiver() {
    //dtor
}


void bellman_ford() {
    for(int i = 1; i < NODE_NUM; i++) {
        int origin = graph[0][i];

        for(int j = 1; j < NODE_NUM; j++) {
            int new_val = graph[0][j] + graph[j][i];
            if(new_val < origin) {
                graph[0][i] = new_val;
                origin = new_val;

                // update routing_table
                routing_table[i].cost = new_val;
                routing_table[i].nexthop = host_names[j].at(0);

                // trigger update!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }
        }
    }
}


void updateTTL(char node) {
    for(int i = 0; i < NODE_NUM; i++) {
        if(routing_table[i].nexthop == node) {
            routing_table[i].ttl = ttl;
        }
    }
}
