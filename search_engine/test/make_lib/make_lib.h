#ifndef TRIM_REDUNT_H
#define TRIM_REDUNT_H
#include "../include/hash.h"
#include "../include/seg.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<cstring>
#include<utility>
#include<string>
#include<unistd.h>
#include<cstdlib>
#include<cmath>
#include<cstdio>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>
#include<dirent.h>
#include<fcntl.h>
#include<locale.h>
#include"../include/conf.h"
#include"../include/mutex.h"
#include<json/json.h>
#define CRITICAL_VALUE 0.6
#define PRIORITY_NUM 20
namespace TRIM_REDUNT
{
    class Trim_redunt
    {
        public:
            static Trim_redunt* getInstance(MY_CONF::CMyconf& conf);
            int get_doc_id(std::string& word,int& doc_id);//获取文档的编号
            void get_doc_title(std::string& word,std::string& title);
            void get_doc_abstract(std::string& word,std::string& abstract);
            void get_content_by_doc_id(const int& doc_id,std::vector<std::string>& content);//通过文档编号获取标题摘要文档内容
            void get_content_by_doc_id(const int& doc_id,std::string& content);//通过文档编号获取文档内容
            void make_page_index(const std::string& index_path);//建立标题摘要文档索引
            void make_page_content_index(const std::string& index_path);//建立文档粗略索引
            void get_page_index();//获取标题摘要文档索引
            void get_page_content_index();//获取文档索引
            void get_doc_num();//获取文档的总数目
            void stop_to_set();//将停用词放入stoplist
            void make_page_title_abstract();//将文档分词,并且制作标题和摘要
            void get_page_word_index(const std::string& page_word_index_path);
            bool doc_redunt(keyword& left,keyword& right)const ;//判断两个文档是否重复
            void remv_dup(const std::string& page_lib_path);//用冒泡排序的思想去重
            void rwrite_page_lib(const std::string& page_lib_path,const std::string& title,const std::string& abstract,const std::string& context,const int& doc_id);//将去重的网页库重新写入磁盘
            void trim_space(std::string& line);//去除换行符等gbk符号
            int gbk2utf8(char *utfstr,const char *srcstr,int maxutfstrlen);//进行转码
            void make_page(const char* src_path,const char* des_path);//将文档内容提取出来制作网页
            void make_page_lib(char* dir_path,char* des_path);//提取目录文档内容制作网页库
            void stoplist_out();
        private:
            Trim_redunt(MY_CONF::CMyconf& m_conf);
            hash_page_index page_index;
            page_content_index p_c_index;
            std::set<std::string> stoplist;
            MixSegment seg;
            MY_CONF::CMyconf& conf;
            int doc_num;
            page_word_index p_w_index;
            page_title_index p_ta_index;
            static Trim_redunt* m_ptr;
            static MUTEX::CMutex m_lock;
            Json::FastWriter writer;
            Json::Reader reader;
            Json::Value root;
    };
}
#endif

