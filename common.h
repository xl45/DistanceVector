#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED


#define INFINITY 16
#define NODE_NUM 4
typedef std::string STR;

#define NODE_A_IP "129.79.242.85"
#define NODE_B_IP "129.79.242.135"
#define NODE_C_IP "129.79.242.73"
#define NODE_D_IP "129.79.242.137"


// update message to send to neighbour (5 bytes)
struct update_msg {
    char node;
    int cost;
};


// each node has a route table, each line of route table is a route_entry
struct route_entry {
    char dst;
    char nexthop;
    int cost;
    int ttl;
};


#endif