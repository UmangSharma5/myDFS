#include<iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include "dataserver.h"

int chunkSize = 2 * 1024 * 1024;

DataServer::DataServer(const string &name):name_(name), buf(nullptr), finish(true){
    string cmd = "mkdir -p " + name_;
    system(cmd.c_str()); // c-style string
}

string DataServer::get_name()const{
    return name_;
}

void DataServer::operator()(){
    while(true){
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [&](){return !this->finish;});
        if (cmd == "put"){
            size_ += bufSize / 1024.0 / 1024.0;
            put();
        }
        else if(cmd == "read")
            read();
        else if(cmd == "locate")
            locate();
        else if(cmd == "fetch")
            fetch();
        this->finish = true;
        lk.unlock();
        cv.notify_all();
    }
}

void DataServer::put(){
    int start = 0;
    std::ofstream os;
    while(start < bufSize){
        int offset = start / chunkSize;
        string filePath = name_ + "/" + to_string(fid) + " " + to_string(offset);
        os.open(filePath);
        if(!os){
            std::cerr << "create file error in dataserver: (file name) " << filePath << std::endl;
        }
        os.write(&buf[start], min(chunkSize, bufSize - start));
        start += chunkSize;
        os.close();
    }
}

void DataServer::read(){
    int start = 0;
    buf = new char[bufSize];
    while(start < bufSize){
        int offset = start / chunkSize;
        string filePath = name_ + "/" + to_string(fid) + " " + to_string(offset);
        std::ifstream is(filePath);
        if(!is){
            delete[] buf;
            bufSize = 0; // This will indicate faliure
            break;
        }
        is.read(&buf[start], min(chunkSize, bufSize - start));
        start += chunkSize;
    }
}

void DataServer::fetch(){
    buf = new char[chunkSize];
    string filePath = name_ + "/" + to_string(fid) + " " + to_string(offset);
    std::ifstream is(filePath);
    if(!is){
        delete[] buf;
        bufSize = 0;
    }
    else{
        is.read(buf, min(chunkSize, bufSize - chunkSize * offset));        
        bufSize = is.tellg();
    }
}

void DataServer::locate(){
    string filePath = name_ + "/" + to_string(fid) + " " + to_string(offset);
    std::ifstream is(filePath);
    if(is){
        bufSize = 1;
    }
    else{
        bufSize = 0;
    }
}
