#ifndef __HASH_H
#define __HASH_H
#include<iostream>
#include<ext/hash_map>
#include<map>
#include<set>
#include<utility>
#include<queue>
#include<vector>
#define HASH_SIZE 10000
struct CMyhash
{
    int operator()(const std::string& obj)const
    {
        __gnu_cxx::hash<const char*> chash;
        return static_cast<int>(chash(obj.c_str()));
    }
};
struct CMypair
{
    std::string m_word;
    int m_cnt;
    CMypair(std::string word,int cnt):m_word(word),m_cnt(cnt){}
    bool operator<(const CMypair& right)const 
    {
        return m_cnt>right.m_cnt;
    }
};
struct Weight
{
    int m_id;
    double m_weight;
    Weight(int id,double weight):m_id(id),m_weight(weight){}
    bool operator<(const Weight& right)const 
    {
        return m_weight<right.m_weight;
    }
};
/*
int not_null(const std::string& word)
{
    std::string line(word);
    int start=0;
    std::string exclude="\a\n\t\r\v\b\f ";
    while((start=(line.find_first_of(exclude,start)))!=std::string::npos)
    {
        line.erase(start, 1);
        start++;
    }
    if(line.size())
    {
        return 1;
    }
    return 0;
}*/
typedef __gnu_cxx::hash_map<std::string,std::set< std::pair<std::string,int> >,CMyhash> strhash;
typedef __gnu_cxx::hash_map<std::string,std::set< std::pair<std::string,int> >,CMyhash>::iterator striter;
typedef __gnu_cxx::hash_map<std::string,std::string,CMyhash> hash_record;
typedef __gnu_cxx::hash_map<std::string,std::string,CMyhash>::iterator piter;
typedef __gnu_cxx::hash_map<std::string,int,CMyhash> word_cache;
typedef __gnu_cxx::hash_map<std::string,int,CMyhash>::iterator word_cache_iter;
typedef __gnu_cxx::hash_map<int,std::pair<int,int> >  page_content_index;
typedef __gnu_cxx::hash_map<int,std::pair<int,int> >::iterator page_content_index_iter;
typedef __gnu_cxx::hash_map<int,std::vector<std::pair<int,int> > > hash_page_index;
typedef __gnu_cxx::hash_map<int,std::vector<std::pair<int,int> > >::iterator page_index_iter;
typedef __gnu_cxx::hash_map<int,std::pair<std::string,std::string> > page_title_index;
typedef __gnu_cxx::hash_map<int,std::pair<std::string,std::string> >::iterator page_title_index_iter;
typedef __gnu_cxx::hash_map<std::string,int,CMyhash> keyword;
typedef __gnu_cxx::hash_map<std::string,int,CMyhash>::iterator keyiter;
typedef __gnu_cxx::hash_map<std::string,double,CMyhash> hash_tf;
typedef __gnu_cxx::hash_map<std::string,double,CMyhash>::iterator tf_iter;
typedef __gnu_cxx::hash_map<std::string,std::map<int,double>,CMyhash> word_page_rindex;
typedef __gnu_cxx::hash_map<std::string,std::map<int,double>,CMyhash>::iterator word_page_rindex_iter;
typedef std::map<int,std::map<std::string,double> > page_weight_rindex;
typedef std::map<int,std::map<std::string,double> >::iterator page_weight_rindex_iter;
typedef std::map<int,keyword> page_word_index;
typedef std::map<int,keyword>::iterator page_word_index_iter;
#endif  // __HASH_H
