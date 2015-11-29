#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include "Sender.h"
#include "Receiver.h"

#define INFINITY 16
#define NODE_NUM 4
typedef std::string STR;


// each node has a route table, each line of route table is a route_entry
struct route_entry {
    STR dst;
    STR nexthop;
    int cost;
    int ttl;
};


// graph for current node
int graph[NODE_NUM][NODE_NUM] = {
    {0       , 1       , 1       , INFINITY},
    {INFINITY, INFINITY, INFINITY, INFINITY},
    {INFINITY, INFINITY, INFINITY, INFINITY},
    {INFINITY, INFINITY, INFINITY, INFINITY}
};
// route table for current node
route_entry routing_table[NODE_NUM];
// global varibles
STR config_filename;
STR port;
int ttl; // in second
int period; // in second
bool split_horizon;
//
STR host_names[NODE_NUM];


// read config file and generate init graph and routing_table
void initialize();
// send an array of update_msg to neighbour
void send_advert();
// handler function for recv thread
void * recv_service(void *arg);
// handler function for send thread
void * send_service(void *arg);
// tool functions
bool str2int(int &i, char const *s);
void myAssert(int code, STR msg);
// print routing table now
void printRT();


// arg: config_filename, port, ttl, period, split_horizon(1 for true, 0 for false)
int main(int argc, char *argv[]) {
    /** command line args */
    if(argc != 6) {
        std::cout << "invalid arguments, valid format config_filename, port, ttl, period, split_horizon(1 for true, 0 for false).\n";
        exit(1);
    }
    config_filename = argv[1];
    port = argv[2];
    STR arg_ttl = argv[3];
    str2int(ttl, arg_ttl.c_str());
    STR arg_period = argv[4];
    str2int(period, arg_period.c_str());
    STR arg_sh = argv[5];
    if( arg_sh == "1" ) {
        split_horizon = true;
    } else {
        split_horizon = false;
    }
    // print test
    // std::cout << config_filename<<", "<<port<<", "<<ttl<<", "<<period<<", "<<split_horizon<<std::endl;

    /** initialize */
    initialize();
    printRT();

    /** start recv thread */
    // set the attributes to detach mode
    pthread_attr_t attr1;
    int ret1;
    ret1 = pthread_attr_init(&attr1);
    ret1 = pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
    pthread_t recv_thread;
    pthread_create(&recv_thread, &attr1, recv_service, NULL);

    /** start send thread */
    // set the attributes to detach mode
    pthread_attr_t attr2;
    int ret2;
    ret2 = pthread_attr_init(&attr2);
    ret2 = pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);
    pthread_t send_thread;
    pthread_create(&send_thread, &attr2, send_service, NULL);

    while(true) {
        usleep(period * 10000000);
    }
}


void * send_service(void *arg) {
    Sender mySenders[NODE_NUM];
    for(int i = 1; i < NODE_NUM; i++) {
        mySenders[i].init(host_names[i], port);
    }

    while(true) {
        // send
        for(int i = 1; i < NODE_NUM; i++) {
            if(routing_table[i].cost < INFINITY) {
                update_msg msg2send;
                msg2send.addr = routing_table[i].dst;
                msg2send.cost = routing_table[i].cost;

                mySenders[i].mySend(&msg2send);
            }
        }

        // sleep
        usleep(period * 1000000);
    }
}


void * recv_service(void *arg) {
    Receiver myReceiver(port);
    myReceiver.myRecv();
}


void initialize() {
    std::ifstream file(config_filename.c_str());

    STR line;

    for(int i = 0; i < NODE_NUM; i++) {
        std::getline(file, line);
        // std::cout << line << std::endl;
        std::istringstream iss(line);

        do {
            STR sub;
            iss >> sub;
            if(!(sub == "yes" || sub == "no") && sub.length() > 1) host_names[i] = sub;
        } while (iss);
    }

    // print test
    // myAssert(1000, "host names arr print test:");
    // for(int i = 0; i < NODE_NUM; i++) {
    //     std::cout << host_names[i]+", ";
    // }
    // std::cout << "\n";

    // init routing table
    routing_table[0].dst = host_names[0];
    routing_table[0].nexthop = host_names[0];
    routing_table[0].cost = 0;
    routing_table[0].ttl = ttl;
    routing_table[1].dst = host_names[1];
    routing_table[1].nexthop = host_names[1];
    routing_table[1].cost = 1;
    routing_table[1].ttl = ttl;
    routing_table[2].dst = host_names[2];
    routing_table[2].nexthop = host_names[2];
    routing_table[2].cost = 1;
    routing_table[2].ttl = ttl;
    routing_table[3].dst = host_names[3];
    routing_table[3].nexthop = "NULL";
    routing_table[3].cost = INFINITY;
    routing_table[3].ttl = ttl;
}


void printRT() {
    myAssert(2000, "routing table now: ");

    std::cout << "----------------------------\n";

    std::cout << "Node, Next Hop, Cost, TTL\n";

    for(int i = 0; i < NODE_NUM; i++) {
        std::cout << routing_table[i].dst.at(0) << ", " << routing_table[i].nexthop.at(0)
        << ", " << routing_table[i].cost << ", " << routing_table[i].ttl << std::endl;
    }

    std::cout << "----------------------------\n";
}


void myAssert(int code, STR msg) {
    std::cout << code << ", " << msg << std::endl;
}


bool str2int (int &i, char const *s) {
    std::stringstream ss(s);
    ss >> i;
    if (ss.fail()) {
        // not an integer
        return false;
    }
    return true;
}
