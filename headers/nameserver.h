#ifndef _NAME_SERVER_H
#define _NAME_SERVER_H

#include<vector>
#include<map>
#include "filetree.h";
#include "dataserver.h"
using namespace std;

class NameServer {
    private: 
        vector<DataServer *> dataservers_;
        FileTree fileTree_;
        int numReplicate_;
        int idCnt_;

        vector<string> parse_cmd();
            
    public:
        std::map<string,pair<int,int>> meta;

        explicit NameServer(int numReplicate);
        void add(DataServer *server);
        void operator()();
};

#endif
