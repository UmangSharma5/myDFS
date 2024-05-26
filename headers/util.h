#ifndef _UTIL_H
#define _UTIL_H

#define BLOCK_SIZE 64

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

///////////////// Referred Github/Google (open source) for MD5/////////////////////

class MD5{
    public:
        MD5();
        MD5& update(const unsigned char* in, size_t inputLen);
        MD5& update(const char* in, size_t inputLen);
        MD5& finalize();
        string toString() const;

    private:
        void init();
        void transform(const uint8_t block[BLOCK_SIZE]);

        uint8_t buffer[BLOCK_SIZE];
        uint8_t digest[16];

        uint32_t state[4];
        uint32_t lo,hi;
        bool finalized;        
};

string md5(const string str);


#endif