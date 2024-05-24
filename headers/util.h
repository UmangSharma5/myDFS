#ifndef _UTIL_H
#define _UTIL_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <array>
#include <iterator>
#include <cstdint>
using namespace std;

vector<string> split(const string &cmd, const char &delim);

template<typename T>
std::vector<int> argsort(const std::vector<T> &vec){
    std::vector<int> idx;
    for(int i=0; i<vec.size(); ++i)
        idx.push_back(i);
    sort(idx.begin(), idx.end(), [&vec](int id1, int id2){return vec[id1] < vec[id2];});
    return idx;
}



#endif