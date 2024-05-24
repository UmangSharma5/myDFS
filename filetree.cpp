#include<iostream>
#include<string>
#include<cmath>
#include"filetree.h"
#include"util.h"
using namespace std;

FileTree::FileTree(){
    TreeNode("/",false);
} 

bool FileTree::find_node(const string &path, TreeNode **last_node)const{
    vector<string> path_folder = split(path,'/');

    TreeNode *node = _root->firstson;
    *last_node = _root;
    for(const auto &name : path_folder){
        while(node && node->value_ != name){
            node = node->nextsibling;
        }

        if(!node) return false;
        
        *last_node = node;
        node = node->firstson;
    }
    return true && node->isFile;
}


bool FileTree::insert_node(const string &path, const bool isFile){
    TreeNode *parent = nullptr;
    bool found = find_node(path,&parent);
    if(found) return false;

    TreeNode *newNode = new TreeNode(path,isFile);
    TreeNode *son = parent->firstson;
    if(!son){
        parent->firstson = newNode;
    }
    else{
        while(son->nextsibling){
            son = son->nextsibling;
        }
        son->nextsibling = newNode;
    }
    return true;
}

void FileTree::list(TreeNode *node, map<string,pair<int,int>> &meta){
    static int chunkSize = 2 * 1024 * 1024;
    if(node){
        cout<<node->value_<<"\t"<<meta[node->value_].first<<"\t"<< (int)ceil(1.0 * meta[node->value_].second/chunkSize) << endl;
        list(node->firstson,meta);
        list(node->nextsibling,meta);
    }
}

void FileTree::list(map<string,pair<int,int>> &meta){
    list(_root,meta);
}
