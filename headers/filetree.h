#ifndef _FILE_TREE_H
#define _FILE_TREE_H

#include <string>
#include <map>
using namespace std;

struct TreeNode{
    string value_;
    bool isFile;
    TreeNode *parent;
    TreeNode *firstson;
    TreeNode *nextsibling;

    TreeNode(const string &_value , const bool &_isFile):value_(_value),isFile(_isFile),parent(nullptr),firstson(nullptr),nextsibling(nullptr) {};
};

class FileTree{
    private:
        TreeNode *_root;
    public:
        FileTree();
        bool insert_node(const string &path, const bool isFile);
        bool find_node(const string &path, TreeNode **parent) const;
        void list(TreeNode *node, map<string,pair<int,int>>& meta);
        void list(map<string,pair<int,int>>& meta);
};

#endif