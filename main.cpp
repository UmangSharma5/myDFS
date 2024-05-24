#include <iostream>
#include <thread>
#include <vector>
#include "nameserver.h";
#include "dataserver.h";

const int numReplicate = 3;

int main(){
    NameServer ns(numReplicate);
    DataServer ds1("node1");
    DataServer ds2("node2");
    DataServer ds3("node3");
    DataServer ds4("node4");
    return 0;
}