#ifndef __PAGE_RANK_H
#define __PAGE_RANK_H
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
#include<json/json.h>
#include"conf.h"
#include"mutex.h"
#include"hash.h"
#include"seg.h"
#define PAGE_RESULT_NUM 10
#define MODEL 30
namespace PAGE_RANK
{
    class Page_rank
    {
        public:
            static Page_rank* getInstance(MY_CONF::CMyconf& m_conf);//静态方法获取实例
            void stoplist_out();
            void trim_space(std::string& line);
            void get_doc_id(std::string& word,int& doc_id);//获取文档的编号
            void get_content_by_doc_id(const int& doc_id,std::vector<std::string>& content);//通过文档编号获取文档内容
            void make_page_index(const std::string& index_path);//建立文档索引
            void get_page_index();//获取文档索引
            void get_doc_num();//获取文档的总数目
            void stop_to_set(std::set<std::string>& stoplist);
            void make_word_page_rindex(const std::string& word_page_rindex_path);//建立词汇文档倒排索引
            void get_word_page_rindex();//获取词汇文档倒排索引
            void make_page_weight_rindex(const std::string& p_w_rindex_path);//建立页面权重倒排索引
            void get_page_weight_rindex();//获取页面倒排索引
            void page_weight_out();//输出页面权重倒排索引
            void make_page_module_index(const std::string& page_module_path);//建立文档权重的模的倒排索引
            void get_page_module_index();//获取文档权重的模倒排索引
            void get_query_tf(const std::string& content);//获取查询词汇的tf
            void get_query_tf_idf();//获取查询向量的tf_idf
            void get_query_module();//获取查询向量的模
            void get_doc_intersec();//两个词汇的共同的交集文档id
            void get_doc_similar();//计算两个文档的相似度
            void page_split(const std::string& content,std::vector<std::string>& seg_page);
            void page_result_out(std::vector<std::vector<std::string> >& result);//根据优先级队列返回搜索结果
            void init();//查询系统初始化
            int operator()(std::string& query,std::string& result,hash_record& title_content);//执行查询任务
        private:
            Page_rank(MY_CONF::CMyconf& m_conf);
            Page_rank();
            Page_rank(const Page_rank& obj);
            Page_rank& operator=(const Page_rank& obj);
            MY_CONF::CMyconf& conf;
            hash_page_index page_index;
            int doc_num;
            MixSegment seg;
            std::set<std::string> stoplist;
            word_page_rindex word_tf_idf;
            page_weight_rindex p_w_rindex;
            std::map<int,double> page_module;
            hash_tf query_tf_idf;
            double query_module;
            std::set<int> doc_id_set;
            std::priority_queue<Weight> page_result;
            Json::FastWriter writer;
            static Page_rank* m_ptr;
            static MUTEX::CMutex m_lock;
    };
}
#endif
