#include <iostream>
#include <string>
#include "nameserver.h"
#include "filetree.h"
#include "util.h"
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;

NameServer::NameServer(int numReplicate): numReplicate_(numReplicate),idCnt_(0){}

void NameServer::add(DataServer *server_){
    dataservers_.push_back(server_);
}   

vector<string> NameServer::parse_cmd() {
    cout<<"MyDFS> ";
    string cmd,temp;
    getline(cin,cmd);
    vector<string>parameters;
    stringstream ss(cmd); //string object with a stream allowing you to read from the string as if it were a stream (like cin)
    while(ss >> temp){
        parameters.push_back(temp);
    }
    return parameters;
}


void NameServer::operator()(){
    while(true){
        vector<string>parameters = parse_cmd();
        vector<int>idx; // store index in asec order 
        char* buf = nullptr;
        std::ifstream is;        

        if(parameters.empty()){
            std::cerr << "input a blank line" << std::endl;
            continue;
        }
        if(parameters[0] == "quit"){
            exit(0);
        }
        // To list all files in name server
        else if(parameters[0] == "list" || parameters[0] == "ls"){
            if(parameters.size() != 1){
                std::cerr << "useage: " << "list (list all the files in name server)" << endl;
            }
            else{
                cout << "file\tfileId\tchunkNumber" << endl;
                fileTree_.list(meta);                
            }
            continue;
        }
        // upload file to myDFS
        else if(parameters[0] == "put"){
            if(parameters.size()!=3){
                std::cerr << "useage: " << "put source_file_path des_file_path" << std::endl;
                continue;
            }
            is.open(parameters[1], ifstream::ate | ifstream::binary);
            if(!is){
                std::cerr << "open file error: file " << parameters[1] << std::endl;
                continue;
            }
            else if (!fileTree_.insert_node(parameters[2], true)){
                std::cerr << "create file error \n.maybe the file : " << parameters[2] << "exists" << std::endl;
                continue;
            }
            else{
                int totalSize = static_cast<int>(is.tellg());
                if(totalSize < 0){
                    std::cerr << "Error determining file size." << std::endl;
                }            

                buf = new char[totalSize];
                is.seekg(0, is.beg);
                is.read(buf, totalSize);

                vector<double>serverSize;
                for(const auto &server : dataservers_){
                    serverSize.push_back(server->size());
                }

                idx = argsort<double>(serverSize);  
                idCnt_++;
                for(int i=0; i<numReplicate_; i++){
                    unique_lock<std::mutex> lk(dataservers_[idx[i]]->mtx);
                    meta[parameters[2]] = make_pair(idCnt_, totalSize);
                    dataservers_[idx[i]]->cmd = "put";
                    dataservers_[idx[i]]->fid = idCnt_;
                    dataservers_[idx[i]]->bufsize = totalSize;
                    dataservers_[idx[i]]->buf = buf;
                    dataservers_[idx[i]]->finish = false;
                    lk.unlock();
                    dataservers_[idx[i]]->cv.notify_all();
                }
            }
        }
    }
}
