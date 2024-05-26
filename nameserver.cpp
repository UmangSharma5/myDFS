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
    dataServers_.push_back(server_);
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

        MD5 md5;    

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
                for(const auto &server : dataServers_){
                    serverSize.push_back(server->size());
                }

                idx = argsort<double>(serverSize);  
                idCnt_++;
                for(int i=0; i<4; i++){
                    unique_lock<std::mutex> lk(dataServers_[idx[i]]->mtx);
                    meta[parameters[2]] = make_pair(idCnt_, totalSize);
                    dataServers_[idx[i]]->cmd = parameters[0];
                    dataServers_[idx[i]]->fid = idCnt_;
                    dataServers_[idx[i]]->bufSize = totalSize;
                    dataServers_[idx[i]]->buf = buf;
                    dataServers_[idx[i]]->finish = false;
                    lk.unlock();
                    dataServers_[idx[i]]->cv.notify_all();
                }
            }
        }
        // fetch file from myDFS
        else if(parameters[0] == "read" || parameters[0] == "fetch"){
            if(parameters.size() != 3 && parameters.size() != 4){
                std::cerr << "useage: " << "read source_file_path dest_file_path" << std::endl;
                std::cerr << "useage: " << "fetch FileID Offset dest_file_path" << std::endl;
                continue;
            }
            else{
                if(parameters[0] == "read" && meta.find(parameters[1]) == meta.end()){
                      std::cerr << "error: no such file in myDFS." << std::endl;
                    continue;
                }
                for(int i=0;i<4;i++){
                    std::unique_lock<std::mutex> lk(dataServers_[i]->mtx);
                    dataServers_[i]->cmd = parameters[0];
                    if(parameters[0] == "read"){
                        std::pair<int,int> metaData = meta[parameters[1]];
                        dataServers_[i]->fid = metaData.first;
                        dataServers_[i]->bufSize = metaData.second;                        
                    }
                    else{
                        dataServers_[i]->fid = std::stoi(parameters[1]);
                        dataServers_[i]->offset = std::stoi(parameters[2]);
                    }
                    dataServers_[i]->finish = false;
                    lk.unlock();
                    dataServers_[i]->cv.notify_all();                    
                }                
            }
        }
        // locate the data server given fileId and offset
        else if(parameters[0] == "locate"){
            if(parameters.size() != 3){
                std::cerr << "useage: " << "locate fileID Offset" << std::endl;
                continue;
            }
            else{
                for(int i=0;i<4;i++){
                    std::unique_lock<std::mutex> lk(dataServers_[i]->mtx);
                    dataServers_[i]->cmd = parameters[0];
                    dataServers_[i]->fid = std::stoi(parameters[1]);
                    dataServers_[i]->offset = std::stoi(parameters[2]);
                    dataServers_[i]->finish = false;
                    lk.unlock();
                    dataServers_[i]->cv.notify_all();
                }
            }
        }
        // any other command will give error (unless i add new XD)
        else{
            std::cerr << "wrong command" <<endl;
        }

        for(const auto & server :dataServers_){
            std::unique_lock<std::mutex> lk(server->mtx);
            (server->cv).wait(lk,[&](){return server->finish;});
            lk.unlock();
            (server->cv).notify_all();
        }

        if(parameters[0] == "fetch" || parameters[0] == "read"){
            string md5_checksum, pre_checksum;
            for(int i=0;i<4;i++){
                if(dataServers_[i]->bufSize){
                    std::ofstream os;
                    if(parameters[0] == "read"){
                        os.open(parameters[2]);
                    }
                    else if(parameters[0] == "fetch"){
                        os.open(parameters[3]);
                    }
                    if(!os){
                        std::cerr << "create file failed. maybe wrong directory." << std::endl;
                    }
                    else{
                        os.write(dataServers_[i]->buf, dataServers_[i]->bufSize);
                        os.close();
                        md5.update(dataServers_[i]->buf, dataServers_[i]->bufSize);
                        md5.finalize();
                        md5_checksum = md5.toString();
                        if(!pre_checksum.empty() && pre_checksum != md5_checksum){
                            std::cerr << "error: unequal checksum for files from different dataServers. File got may be wrong." << std::endl;
                        }
                        pre_checksum = md5_checksum;
                    }
                     delete[] dataServers_[i]->buf;   
                }
            }
        }
        else if(parameters[0] == "put"){
            std::cout << "Upload success. The file ID is " << idCnt_ << std::endl;
        }
        else if(parameters[0] == "locate" || parameters[0] == "ls"){
            bool notFound = true;
            for(int i=0;i<4;i++){
                if(dataServers_[i]->bufSize){
                    notFound = false;
                    std::cout << "found FileID " << parameters[1] << " offset " << parameters[2] << " at " << dataServers_[i]->get_name() << std::endl;
                }
            }
            if(notFound){
                std::cout << "not found FileID " << parameters[1] << " offset " << parameters[2] << std::endl;
            }
        }
        
        delete[] buf;
        is.close();
    }
}
