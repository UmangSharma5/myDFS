#ifndef _DATA_SERVER_H
#define _DATA_SERVER_H

#include <string>
#include <mutex>
#include <condition_variable>
using namespace std;

extern int chunkSize;

class DataServer{
    private:
        string name_;
        double size_;

        void put();
        void read();
        void locate();
        void fetch();
    public:
        mutex mtx;
        condition_variable cv;
        string cmd;
        int fid, bufSize, offset;
        char* buf;
        bool finish;

        DataServer(const string &name);
        void operator()();
        double size()const{return size_;}
        string get_name()const;
};

#endif