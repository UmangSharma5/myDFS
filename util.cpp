#include "util.h"
#include<iostream>
using namespace std;


vector<string> split(const string &s, const char &delim){
    stringstream ss(s);
    vector<string> result;
    string str;
    while(getline(ss,str,delim)){
        if(!str.empty()){
            result.push_back(str);
        }
    }
    return result; 
}



