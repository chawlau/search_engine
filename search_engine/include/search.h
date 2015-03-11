#ifndef __SEARCH_H
#define __SEARCH_H
#include<iostream>
#include<fstream>
#include<sstream>
#include<utility>
#include<set>
#include<map>
#include<queue>
#include<cstring>
#include<json/json.h>
#include"dis.h"
#include"hash.h"
#define HASH_BASE 10000
namespace WORD_CORRECT
{
    struct CMypair
    {
        std::string m_str;
        int m_freq;
        int m_dis;
        CMypair(std::string str="",int freq=0,int dis=0):m_str(str),m_freq(freq),m_dis(dis){}
        bool operator<(const CMypair& right) const;
        void pair_out();
    };
    class CWord_correct
    {
        private:
            const char* dic_path;
            strhash r_index;
        public:
            DISTANCE::Edit edit;
            CWord_correct(const char*  path);
            void index_out(strhash& r_index);
            void build(const std::string& str,const int& cnt,strhash& r_index);
            void split_str(const std::string& str,std::vector<std::string>& dest);
            void make_index(strhash& r_index,const char* dic_path);
            void make_cache(word_cache& cache,strhash& r_index,const std::string& search_word);
            void cache_out(word_cache& cache);
            void get_correct(word_cache& cache,std::vector<CMypair>& re_vec,const std::string& search_word);
            void operator()(const std::string& search_word,std::string& search_result);
    };
}
#endif
