#include"page_rank.h"
PAGE_RANK::Page_rank* PAGE_RANK::Page_rank::m_ptr=NULL;
MUTEX::CMutex PAGE_RANK::Page_rank::m_lock;
namespace PAGE_RANK
{
    Page_rank* Page_rank::getInstance(MY_CONF::CMyconf& conf)
    {
        if(m_ptr==NULL)
        {
            m_lock.lock();
            if(m_ptr==NULL)
            {
                m_ptr=new Page_rank(conf);
            }
            m_lock.unlock();
        }
        return m_ptr;
    }
    Page_rank::Page_rank(MY_CONF::CMyconf& m_conf):conf(m_conf),doc_num(0),seg(JIEBA_DICT_FILE, HMM_DICT_FILE, USER_DICT_FILE)
    {
        stop_to_set(stoplist);
    }
    void Page_rank::stoplist_out()
    {
        std::set<std::string>::iterator iter=stoplist.begin();
        for(iter;iter!=stoplist.end();iter++)
        {
            std::cout<<*iter<<std::endl;
        }
    }
    void Page_rank::get_doc_id(std::string& word,int& doc_id)
    {
        if(word.find("<docid>",0)!=std::string::npos)
        {
            std::stringstream ss; 
            int bg=word.find(">",0);
            int ed=word.find("</",0);
            ss<<word.substr(bg+1,ed-bg-1);
            ss>>doc_id;
        }
    }
    void Page_rank::make_page_index(const std::string& index_path)
    {
        std::ifstream fin(conf.config["my_page_lib"].c_str());
        std::ofstream fout(index_path.c_str());
        std::string line;
        int doc_id=0,offset=0,content_len=0,begin=0,end=0;
        int t_offset=0,title_len=0,a_offset=0,abstract_len=0;
        int t_begin=0,t_end=0,a_begin=0,a_end=0;
        while(getline(fin,line))
        {
            get_doc_id(line,doc_id);
            if(line.find("<title>",0)!=std::string::npos)
            {
                t_begin=fin.tellg();
            }
            if(line.find("</title>",0)!=std::string::npos)
            {
                t_end=fin.tellg();
                title_len=t_end-strlen("</title>\n")-t_begin;
                t_offset=t_begin;
            }
            if(line.find("<abstract>",0)!=std::string::npos)
            {
                a_begin=fin.tellg();
            }
            if(line.find("</abstract>",0)!=std::string::npos)
            {
                a_end=fin.tellg();
                abstract_len=a_end-strlen("</abstract>\n")-a_begin;
                a_offset=a_begin;
            }
            if(line.find("<content>",0)!=std::string::npos)
            {
                begin=fin.tellg();
            }
            if(line.find("</content>",0)!=std::string::npos)
            {
                end=fin.tellg();
                content_len=end-strlen("</content>\n")-begin;
                offset=begin;
                fout<<doc_id<<" "<<t_offset<<" "<<title_len<<" ";
                fout<<a_offset<<" "<<abstract_len<<" ";
                fout<<offset<<" "<<content_len<<std::endl;
                t_offset=0,title_len=0,t_begin=0,t_end=0;
                a_offset=0,abstract_len=0,a_begin=0,a_end=0;
                doc_id=0,offset=0,content_len=0,begin=0,end=0;
            }
        }
        fin.close();
        fout.close();
    }
    void Page_rank::get_page_index()
    {
        std::ifstream findex(conf.config["my_content_index"].c_str());
        std::string line;
        while(getline(findex,line))
        {
            std::vector<int> temp;
            std::vector<std::pair<int,int> > content_vec;
            std::istringstream ss(line);
            int data;
            while(ss>>data)
            {
                temp.push_back(data);
            }
            content_vec.push_back(make_pair(temp[1],temp[2]));
            content_vec.push_back(make_pair(temp[3],temp[4]));
            content_vec.push_back(make_pair(temp[5],temp[6]));
            std::pair<int,std::vector<std::pair<int,int> > > pair2=make_pair(temp[0],content_vec);
            page_index.insert(pair2);
        }
        findex.close();
    }
    void Page_rank::get_doc_num()
    {
        std::ifstream fin(conf.config["my_content_index"].c_str());
        std::string line;
        while(getline(fin,line))
        {
            doc_num++;
        }
    }
    void Page_rank::get_content_by_doc_id(const int& doc_id,std::vector<std::string>& content)
    {
        std::string temp;
        std::ifstream fsrc(conf.config["my_page_lib"].c_str());
        char* context =new char[5000000]();
        for(int index=0;index!=3;index++)
        {
            bzero(context,5000000);
            fsrc.seekg(page_index[doc_id][index].first); //将文件流指针定位
            fsrc.read(context,page_index[doc_id][index].second);//根据索引读取文件
            temp.assign(context);
            content.push_back(temp);
            temp.clear();
        }
        delete[] context;
        fsrc.close();
    }
    void Page_rank::stop_to_set(std::set<std::string>& stoplist)
    {
        std::string line;
        std::ifstream stop(conf.config["my_stoplist"].c_str());
        while(getline(stop,line))
        {
            std::string temp;
            std::istringstream ss(line);
            ss>>temp;
            stoplist.insert(temp);
        }
        stop.close();
    }
    void Page_rank::make_word_page_rindex(const std::string& word_page_rindex_path)
    {
        std::ifstream fsrc(conf.config["my_page_lib"].c_str());
        int total_doc=0;
        keyword word_in_doc;
        double word_IDF=0;
        word_page_rindex  word_tf_idf;
        std::map<int,double> word_TF;
        page_index_iter iter=page_index.begin();
        std::string exclude="\a\v\b\f\n\t\r ";
        for(iter;iter!=page_index.end();iter++)//遍历存放索引文件的hash_map，获取文档内容
        {
            total_doc++; //总文档数目
            std::cout<<"doc_id"<<iter->first<<"'s task"<<std::endl;
            keyword doc_word_freq; //存放一个文档的词汇      
            fsrc.seekg((iter->second)[2].first); //将文件流指针定位
            char* content =new char[5000000]();
            fsrc.read(content,(iter->second)[2].second);//根据索引读取文件
            std::vector<std::string> word; //存放提取的文档内容       
            seg.cut(content,word);         //提取的文档内容分词
            int word_num_exclude=0;//排除一些空格和换页换行符号的文档词汇数目
            std::vector<std::string>::iterator iter1=word.begin();
            for(iter1;iter1!=word.end();iter1++) //将提取的分词存放到doc_word_freq中
            {
                if(stoplist.count(*iter1)>0||(*iter1).find_first_of(exclude,0)!=std::string::npos)    
                {
                    continue;
                }
                word_num_exclude++;
                doc_word_freq[*iter1]++;
            }
            keyiter word_iter=doc_word_freq.begin();
            for(word_iter;word_iter!=doc_word_freq.end();word_iter++)
            {
                //double doc_TF=(double)(word_iter->second)/(double)word.size();
                double doc_TF=(double)(word_iter->second)/(double)word_num_exclude;
                std::pair<int,double> apair=make_pair(iter->first,doc_TF);
                word_tf_idf[word_iter->first].insert(apair);//将提取词汇按string:doc_id:doc_TF存储在word_tf_idf中
            }
            doc_word_freq.clear();
            delete[] content;
            word.clear();
        }
        word_page_rindex_iter index_iter=word_tf_idf.begin();
        std::map<int,double> doc_weight_index;
        for(index_iter;index_iter!=word_tf_idf.end();index_iter++)//计算word_tf_idf中word出现文档的数目,将doc_TF替换成权重
        {
            word_IDF=log((double)total_doc/((double)word_tf_idf[index_iter->first].size()+1));
            std::map<int,double>::iterator iter=(index_iter->second).begin();
            for(iter;iter!=(index_iter->second).end();iter++)
            {
                double weight=(iter->second)*word_IDF;
                iter->second=weight;  //计算word 对应的map串中对应的每个doc_id中weight,
                doc_weight_index[iter->first]+=weight;//将对应的doc_id中的weight相应地求和，求出doc_id对应的总权重
            }
        }
        index_iter=word_tf_idf.begin();
        for(index_iter;index_iter!=word_tf_idf.end();index_iter++)//计算word_tf_idf中word在每个文档的相对权重
        {
            std::map<int,double>::iterator iter=(index_iter->second).begin();
            for(iter;iter!=(index_iter->second).end();iter++)
            {
                double r_weight=(iter->second)/(doc_weight_index[iter->first]);
                iter->second=r_weight;  //计算word 对应的map串中对应的每个doc_id中相对weight,
            }
        }
        index_iter=word_tf_idf.begin();//将倒排索引写入磁盘文件
        std::ofstream findex(word_page_rindex_path.c_str());
        std::cout<<"write word_page_rindex into disk"<<std::endl;
        for(index_iter;index_iter!=word_tf_idf.end();index_iter++)
        {
            if((index_iter->first).size()==0||(index_iter->first).size()<3)
            {
                continue;
            }
            findex<<index_iter->first<<" ";
            std::priority_queue<Weight> top10;
            std::map<int,double>::iterator iter=(index_iter->second).begin();
            for(iter;iter!=(index_iter->second).end();iter++)//将每个词后面的map对象放入优先级队列
            {
                findex<<iter->first<<" "<<iter->second<<" ";
            }
            findex<<std::endl;
        }
        findex.close();
        fsrc.close();
        return;
    }
    void Page_rank::get_word_page_rindex()
    {
        std::ifstream fin(conf.config["my_word_page_rindex"].c_str());
        std::string line;
        int cnt=0;
        while(getline(fin,line))
        {
            if(line.empty()||line.size()<3)
            {
                continue;
            }
            int pos=line.find(" ");
            std::string str1=line.substr(0,pos);
            std::string str2=line.substr(pos+1,line.size()-pos-1);
            std::istringstream ss(str2);
            int doc_id=0;
            double weight=0;
            while(ss>>doc_id>>weight)
            {
                std::pair<int,double> apair=make_pair(doc_id,weight);
                word_tf_idf[str1].insert(apair);
            }
            //std::cout<<"put "<<++cnt<<" : word_page_rindex int hash_map"<<std::endl;
        }
        std::cout<<"word_page_rindex hash been established"<<std::endl;
    }
    void Page_rank::make_page_weight_rindex(const std::string& p_w_rindex_path)
    {
        page_weight_rindex p_w_rindex;
        word_page_rindex_iter iter=word_tf_idf.begin();
        int id_num=0;
        for(iter;iter!=word_tf_idf.end();iter++)
        {
            std::map<int,double>::iterator iter1=(iter->second).begin();
            for(iter1;iter1!=(iter->second).end();iter1++)
            {
                std::pair<std::string,double> apair=make_pair(iter->first,iter1->second);
                p_w_rindex[iter1->first].insert(apair);
            }
            std::cout<<++id_num<<" word is done"<<std::endl;
        }
        page_weight_rindex_iter p_w_riter=p_w_rindex.begin();
        std::ofstream fout(p_w_rindex_path.c_str());
        for(p_w_riter;p_w_riter!=p_w_rindex.end();p_w_riter++)
        {
            if((p_w_riter->second).size()==0)
            {
                continue;
            }
            fout<<p_w_riter->first<<" ";
            std::map<std::string,double>::iterator iter1=(p_w_riter->second).begin();
            for(iter1;iter1!=(p_w_riter->second).end();iter1++)
            {
                fout<<iter1->first<<" "<<iter1->second<<" ";
            }
            fout<<std::endl;
            std::cout<<p_w_riter->first<<" doc is done"<<std::endl;
        }
        fout.close();
    }
    void Page_rank::get_page_weight_rindex()
    {
        std::ifstream fin(conf.config["my_page_weight_rindex"].c_str());
        std::string line;
        int cnt=0;
        while(getline(fin,line))
        {
            if(line.size()==0||line.size()<3)
            {
                continue;
            }
            int pos=line.find(" ");
            std::string str1=line.substr(0,pos);
            int doc_id=0;
            std::istringstream ss1(str1);
            ss1>>doc_id;
            std::string str2=line.substr(pos+1,line.size()-pos-1);
            std::istringstream ss(str2);
            std::string word;
            double weight=0;
            while(ss>>word>>weight)
            {
                std::pair<std::string,double> apair=make_pair(word,weight);
                p_w_rindex[doc_id].insert(apair);
            }
            //std::cout<<"put "<<++cnt<<" : page_weight_rindex int map"<<std::endl;
        }
        std::cout<<"page_weight_rindex hash been established"<<std::endl;
    }
    void Page_rank::page_weight_out()
    {
        page_weight_rindex_iter p_w_riter=p_w_rindex.begin();
        int num=0;
        for(p_w_riter;p_w_riter!=p_w_rindex.end(),num<20;p_w_riter++,num++)
        {
            std::cout<<p_w_riter->first<<" ";
            std::map<std::string,double>::iterator iter1=(p_w_riter->second).begin();
            for(iter1;iter1!=(p_w_riter->second).end();iter1++)
            {
                std::cout<<iter1->first<<" "<<iter1->second<<" ";
            }
            std::cout<<std::endl;
            std::cout<<p_w_riter->first<<" doc is done"<<std::endl;
        }
    }
    void Page_rank::make_page_module_index(const std::string& page_module_path)
    {
        page_weight_rindex_iter p_w_riter=p_w_rindex.begin();
        std::map<int,double> page_module;
        for(p_w_riter;p_w_riter!=p_w_rindex.end();p_w_riter++)
        {
            std::map<std::string,double>::iterator iter1=(p_w_riter->second).begin();
            double squ_value=0;
            for(iter1;iter1!=(p_w_riter->second).end();iter1++)
            {
                squ_value+=(iter1->second)*(iter1->second);
            }
            double sqr_value=sqrt(squ_value);
            page_module[p_w_riter->first]=sqr_value;
            std::cout<<p_w_riter->first<<" doc is done"<<std::endl;
        }
        std::ofstream fout(page_module_path.c_str());
        std::map<int,double>::iterator mod_iter=page_module.begin();
        for(mod_iter;mod_iter!=page_module.end();mod_iter++)
        {
            fout<<mod_iter->first<<" "<<mod_iter->second<<std::endl;
        }
        fout.close();
    }
    void Page_rank::get_page_module_index()
    {
        std::ifstream fin(conf.config["my_page_module_index"].c_str());
        std::string line;
        while(getline(fin,line))
        {
            std::istringstream ss(line);
            int doc_id;
            double sqr_value;
            while(ss>>doc_id>>sqr_value)
            {
                page_module[doc_id]=sqr_value;    

            }
        }
        std::cout<<"page_module_index has been establised"<<std::endl;
    }
    void Page_rank::get_query_tf(const std::string& content)
    {
        std::vector<std::string> word; //存放提取的文档内容       
        seg.cut(content,word);         //提取的文档内容分词
        std::vector<std::string>::iterator iter1=word.begin();
        keyword doc_word_freq;    
        int exclude_num=0;
        std::string exclude="\a\v\b\f\n\t\r ";
        for(iter1;iter1!=word.end();iter1++) //将提取的分词存放到doc_word_freq中
        {
            if(stoplist.count(*iter1)>0||(*iter1).find_first_of(exclude,0)!=std::string::npos)    
            {
                continue;
            }
            doc_word_freq[*iter1]++;
            exclude_num++;
        }
        keyiter word_iter=doc_word_freq.begin();
        for(word_iter;word_iter!=doc_word_freq.end();word_iter++)
        {
            //double doc_TF=(double)(word_iter->second)/(double)word.size();
            double doc_TF=(double)(word_iter->second)/(double)exclude_num;
            std::pair<std::string,double> apair=make_pair(word_iter->first,doc_TF);
            query_tf_idf.insert(apair);//将提取词汇按string:doc_TF存储在query_tf_idf中
        }
        doc_word_freq.clear();
        word.clear();
    }
    void Page_rank::get_query_tf_idf()
    {
        double total_weight;
        tf_iter tf_iter1=query_tf_idf.begin();
        for(tf_iter1;tf_iter1!=query_tf_idf.end();tf_iter1++)//计算查询文档的tf_idf
        {
            double word_IDF=log((double)(doc_num+1)/((double)word_tf_idf[tf_iter1->first].size()+2));
            tf_iter1->second=(tf_iter1->second)*word_IDF;
            total_weight+=tf_iter1->second;//查询文档的总权重
        }
        tf_iter1=query_tf_idf.begin();
        for(tf_iter1;tf_iter1!=query_tf_idf.end();tf_iter1++)//计算查询文档的相对权重tf_idf
        {
            tf_iter1->second=(tf_iter1->second)/total_weight;
        }
    }
    void Page_rank::get_query_module()//获取查询向量的模
    {
        tf_iter query_iter=query_tf_idf.begin();
        double squ_value=0;
        for(query_iter;query_iter!=query_tf_idf.end();query_iter++)
        {
            squ_value+=(query_iter->second)*(query_iter->second);
        }
        query_module=sqrt(squ_value);
    }
    void Page_rank::get_doc_intersec()
    {
        page_weight_rindex_iter p_w_riter=p_w_rindex.begin();
        for(p_w_riter;p_w_riter!=p_w_rindex.end();p_w_riter++)
        {
            doc_id_set.insert(p_w_riter->first);
        }
        tf_iter query_iter=query_tf_idf.begin();
        for(query_iter;query_iter!=query_tf_idf.end();query_iter++)
        {
            std::set<int>::iterator s_iter=doc_id_set.begin();
            std::set<int> temp_set;
            for(s_iter;s_iter!=doc_id_set.end();s_iter++)
            {
                if(p_w_rindex[*s_iter].count(query_iter->first)>0)
                {
                    temp_set.insert(*s_iter);
                }
            }
            doc_id_set.swap(temp_set);
            temp_set.clear();
        }
    }
    void Page_rank::get_doc_similar()
    {
        std::set<int>::iterator iter=doc_id_set.begin();
        for(iter;iter!=doc_id_set.end();iter++)
        {
            tf_iter query_iter=query_tf_idf.begin();
            double cross_product=0;
            for(query_iter;query_iter!=query_tf_idf.end();query_iter++)
            {
                cross_product+=(p_w_rindex[*iter][query_iter->first])*(query_iter->second);
            }
            double multi_module=query_module*(page_module[*iter]);
            double doc_similar=cross_product/multi_module;
            Weight* weight=new Weight(*iter,doc_similar);
            page_result.push(*weight);
        }
    }
    void Page_rank::page_split(const std::string& content,std::vector<std::string>& seg_page)
    {
        std::vector<std::string> page;
        seg.cut(content,page);
        int gap=page.size()/MODEL;
        std::vector<std::string>::iterator iter=page.begin();
        std::string context;
        int seg_cnt=0;
        for(size_t index=0;index!=page.size();index++)
        {
            context.append(page[index]);
            if((index+1)%gap==0||index==page.size()-1)
            {
                if(++seg_cnt==MODEL&&index!=page.size()-1)
                {
                    continue;
                }
                seg_page.push_back(context);   
                context.clear();
            }
        }
    }
    void Page_rank::page_result_out(std::vector<std::vector<std::string> >& result)
    {
        while(page_result.size())
        {
            //std::cout<<page_result.top().m_id<<"---->"<<page_result.top().m_weight<<std::endl;
            //std::cout<<"----------------------------------------------------------------------"<<std::endl;
            std::vector<std::string> seg_page;
            get_content_by_doc_id(page_result.top().m_id,seg_page);
            result.push_back(seg_page);
            seg_page.clear();
            page_result.pop();
        }
        query_tf_idf.clear();
        query_module=0;
        doc_id_set.clear();
    }
    void Page_rank::init()
    {
        get_page_index();
        get_doc_num();
        get_word_page_rindex();
        get_page_weight_rindex();
        get_page_module_index();
        std::cout<<"page_rank system has been established"<<std::endl;
    }
    int Page_rank::operator()(std::string& query,std::string& page_result,hash_record& title_content)
    {
        std::vector<std::vector<std::string> > result;
        get_query_tf(query);
        get_query_tf_idf();
        get_query_module();
        get_doc_intersec();
        get_doc_similar();
        page_result_out(result);
        Json::Value cache;
        if(result.size()!=0)
        {
            std::vector<std::vector<std::string> >::iterator iter=result.begin();
            for(iter;iter!=result.end();iter++)
            {
                Json::Value search_cache;
                std::string title=(*iter)[0];
                trim_space(title);
                search_cache["title"]=title;
                search_cache["abstract"]=(*iter)[1];
                cache.append(search_cache);
                std::pair<std::string,std::string> apair=make_pair(title,(*iter)[2]);
                title_content.insert(apair);
            }
            page_result=writer.write(cache);
            return 1;
        }
        else
        {
            Json::Value err_cache;
            std::string err_title="你所查询的页面不存在或者已经被删除!";
            std::string err_abstract="请检查你的查询输入!";
            err_cache["title"]=err_title;
            err_cache["abstract"]=err_abstract;
            cache.append(err_cache);
            std::pair<std::string,std::string> apair=make_pair(err_title,err_title);
            title_content.insert(apair);
            page_result=writer.write(cache);
            //page_result=writer.write(search_cache);
            return 0;
        }
    }
    void Page_rank::trim_space(std::string& line)    
    {
        int start=0;
        std::string exclude="\a\n\t\r\v\b\f ";
        while((start=(line.find_first_of(exclude,start)))!=std::string::npos)
        {
            line.erase(start, 1);
            start++;
        }
    }
}
