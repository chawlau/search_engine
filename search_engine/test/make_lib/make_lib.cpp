#include "make_lib.h"
TRIM_REDUNT::Trim_redunt* TRIM_REDUNT::Trim_redunt::m_ptr=NULL;
MUTEX::CMutex TRIM_REDUNT::Trim_redunt::m_lock;
namespace TRIM_REDUNT
{
    Trim_redunt* Trim_redunt::getInstance(MY_CONF::CMyconf& conf)
    {
        if(m_ptr==NULL)
        {
            m_lock.lock();
            if(m_ptr==NULL)
            {
                m_ptr=new Trim_redunt(conf);
            }
            m_lock.unlock();
        }
        return m_ptr;
    }
    Trim_redunt::Trim_redunt(MY_CONF::CMyconf& m_conf):conf(m_conf),doc_num(0),seg(JIEBA_DICT_FILE, HMM_DICT_FILE, USER_DICT_FILE)
    {
        stop_to_set();
    }
    void Trim_redunt::stoplist_out()
    {
        std::set<std::string>::iterator iter=stoplist.begin();
        for(iter;iter!=stoplist.end();iter++)
        {
            std::cout<<*iter<<std::endl;
        }
    }
    int Trim_redunt::get_doc_id(std::string& word,int& doc_id)
    {
        if(word.find("<docid>",0)!=std::string::npos)
        {
            std::stringstream ss; 
            int bg=word.find(">",0);
            int ed=word.find("</",0);
            ss<<word.substr(bg+1,ed-bg-1);
            ss>>doc_id;
            return 1;
        }
        return 0;
    }
    void Trim_redunt::get_doc_num()
    {
        doc_num=0;
        std::ifstream fin(conf.config["my_content_index"].c_str());
        std::string line;
        while(getline(fin,line))
        {
            doc_num++;
        }
        std::cout<<"doc_num is : "<<doc_num<<std::endl;
    }
    void Trim_redunt::get_content_by_doc_id(const int& doc_id,std::vector<std::string>& content)
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
    void Trim_redunt::get_content_by_doc_id(const int& doc_id,std::string& content)
    {
        std::ifstream fsrc(conf.config["my_page_lib"].c_str());
        char* context =new char[5000000]();
        fsrc.seekg(p_c_index[doc_id].first); //将文件流指针定位
        fsrc.read(context,p_c_index[doc_id].second);//根据索引读取文件
        content.assign(context);
        delete[] context;
        fsrc.close();
    }
    void Trim_redunt::make_page_content_index(const std::string& index_path)
    {
        std::ifstream fin(conf.config["my_page_lib"].c_str());
        std::ofstream fout(index_path.c_str());
        std::string line;
        int doc_id=0,offset=0,content_len=0,begin=0,end=0;
        while(getline(fin,line))
        {
            get_doc_id(line,doc_id);
            if(line.find("<content>",0)!=std::string::npos)
            {
                begin=fin.tellg();
            }
            if(line.find("</content>",0)!=std::string::npos)
            {
                end=fin.tellg();
                content_len=end-strlen("</content>\n")-begin;
                offset=begin;
                fout<<doc_id<<" "<<offset<<" "<<content_len<<std::endl;
                doc_id=0,offset=0,content_len=0,begin=0,end=0;
            }
        }
        fin.close();
        fout.close();
    }
    void Trim_redunt::make_page_index(const std::string& index_path)
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
    void Trim_redunt::get_page_index()
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
    void Trim_redunt::get_page_content_index()
    {
        std::ifstream findex(conf.config["my_content_index"].c_str());
        std::string line;
        while(getline(findex,line))
        {
            std::vector<int> temp;
            std::istringstream ss(line);
            int data;
            while(ss>>data)
            {
                temp.push_back(data);
            }
            std::pair<int,std::pair<int,int> >  apair=make_pair(temp[0],make_pair(temp[1],temp[2]));
            p_c_index.insert(apair);
        }
        findex.close();
    }
    void Trim_redunt::stop_to_set()
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
    void Trim_redunt::make_page_title_abstract()//制作标题，摘要
    {
        std::ifstream fsrc(conf.config["my_page_lib"].c_str());
        page_content_index_iter iter=p_c_index.begin();
        std::string exclude="\n\t\r〔〕【】 ";
        for(iter;iter!=p_c_index.end();iter++)//遍历存放索引文件的hash_map，获取文档内容
        {
            std::cout<<"handle doc_id: "<<iter->first<<"'s task"<<std::endl;
            keyword doc_word_freq; //存放一个文档的词汇      
            fsrc.seekg((iter->second).first); //将文件流指针定位
            char* content =new char[5000000]();
            fsrc.read(content,(iter->second).second);//根据索引读取文件
            std::vector<std::string> word; //存放提取的文档内容       
            seg.cut(content,word);         //提取的文档内容分词
            std::vector<std::string>::iterator iter1=word.begin();
            for(iter1;iter1!=word.end();iter1++) //将一个提取的分词存放到doc_word_freq中
            {
                if(stoplist.count(*iter1)>0||(*iter1).find_first_of(exclude,0)!=std::string::npos)
                {
                    continue;
                }
                doc_word_freq[*iter1]++;
            }
            std::priority_queue<CMypair> top30;
            keyiter dwf_iter=doc_word_freq.begin();
            for(dwf_iter;dwf_iter!=doc_word_freq.end();dwf_iter++)//将每个文档的词放入优先级队列
            {
                CMypair* apair=new CMypair(dwf_iter->first,dwf_iter->second);
                if(top30.size()<PRIORITY_NUM)
                {
                    top30.push(*apair);
                }
                else
                {
                    if(top30.top()<*apair)
                    {
                        continue;
                    }
                    else
                    {
                        top30.pop();
                        top30.push(*apair);
                    }
                }
            }
            std::vector<std::string> word_freq_vec;
            while(top30.size())//将优先级队列中的元素弹出放入p_w_index 
            {
                std::pair<std::string,int> w_pair=make_pair(top30.top().m_word,top30.top().m_cnt);
                word_freq_vec.push_back(top30.top().m_word);
                p_w_index[iter->first].insert(w_pair);
                top30.pop();
            }
            std::string title;
            std::string abstract;
            for(int index=0;index!=word_freq_vec.size();index++)
            {
                abstract.append(word_freq_vec[index]);
                if(index>14)
                {
                    title.append(word_freq_vec[index]);
                }
            }
            std::pair<std::string,std::string> title_ab=make_pair(title,abstract);
            p_ta_index[iter->first]=title_ab;
            doc_word_freq.clear();
            delete[] content;
        }
        fsrc.close();
    }
    void Trim_redunt::get_page_word_index(const std::string& page_word_index_path)
    {
        std::ifstream fin(page_word_index_path.c_str());
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
            int word_num=0;
            while(ss>>word>>word_num)
            {
                std::pair<std::string,int> apair=make_pair(word,word_num);
                p_w_index[doc_id].insert(apair);
            }
            std::cout<<"put "<<++cnt<<" page_word_index int p_w_index"<<std::endl;
        }
    }
    bool Trim_redunt::doc_redunt(keyword& left,keyword& right)const
    {
        std::set<std::string> common;
        keyiter l_iter=left.begin();
        l_iter=left.begin();
        for(l_iter;l_iter!=left.end();l_iter++)
        {
            if(right.count(l_iter->first)>0)
            {
                common.insert(l_iter->first);
            }
        }
        double percent=(double)common.size()/(double)left.size();
        return percent>CRITICAL_VALUE;
    }
    void Trim_redunt::remv_dup(const std::string& page_lib_path)
    {
        std::vector<int> flag(doc_num+1,1);
        for(int index_i=1;index_i!=flag.size();index_i++)
        {
            for(int index_j=index_i+1;index_j!=flag.size();index_j++)
            {
                if(doc_redunt(p_w_index[index_i],p_w_index[index_j]))
                {
                    std::cout<<index_i<<"and"<<index_j<<" is duplicate"<<std::endl;
                    flag[index_j]=0;
                }
                else
                {
                    std::cout<<index_i<<"and"<<index_j<<" is not duplicate"<<std::endl;
                }
            }
        }
        for(int iter=1,cnt=1;iter!=flag.size();iter++)
        {
            if(flag[iter]!=0)
            {
                std::string context;
                get_content_by_doc_id(iter,context);
                std::string title=p_ta_index[iter].first;
                std::string abstact=p_ta_index[iter].second;
                rwrite_page_lib(page_lib_path,title,abstact,context,cnt);
                cnt++;
                context.clear();
            }
        }
    }
    void Trim_redunt::rwrite_page_lib(const std::string& page_lib_path,const std::string& title,const std::string& abstract,const std::string& context,const int& doc_id)
    {
        std::cout<<"rwrite page_lib"<<std::endl;
        std::ofstream fout;
        fout.open(page_lib_path.c_str(),std::ofstream::out | std::ofstream::app); 
        fout<<"<doc>"<<std::endl;
        fout<<"<docid>"<<doc_id<<"</docid>"<<std::endl;
        fout<<"<url>"<<"http://www.google.com"<<"</url>"<<std::endl;
        fout<<"<title>"<<std::endl;
        fout<<title<<std::endl;
        fout<<"</title>"<<std::endl;
        fout<<"<abstract>"<<std::endl;
        fout<<abstract<<std::endl;
        fout<<"</abstract>"<<std::endl;
        fout<<"<content>"<<std::endl;
        fout<<context;
        fout<<"</content>"<<std::endl;
        fout<<"</doc>"<<std::endl;
        std::cout<<"doc_id"<<doc_id<<"is put in new page_lib"<<std::endl;
        fout.close();
    }
    void Trim_redunt::trim_space(std::string& line)    
    {
        int start=0;
        std::string exclude="\a\n\t\r\v\b\f";
        while((start=(line.find_first_of(exclude,start)))!=std::string::npos)
        {
            line.erase(start,1);
            start++;
        }
    }
    int Trim_redunt::gbk2utf8(char *utfstr,const char *srcstr,int maxutfstrlen)
    {
        if(NULL==srcstr)
        {
            printf("bad parameter\n");
            return -1;
        }

        if(NULL==setlocale(LC_ALL,"zh_CN.gbk"))
        {
            printf("bad parameter\n");
            return -1;
        }
        int unicodelen=mbstowcs(NULL,srcstr,0);
        if(unicodelen<=0)
        {
            printf("can not transfer!!!\n");
            return -1;
        }
        wchar_t *unicodestr=(wchar_t *)calloc(sizeof(wchar_t),unicodelen+1);
        mbstowcs(unicodestr,srcstr,strlen(srcstr));
        if(NULL==setlocale(LC_ALL,"zh_CN.utf8"))
        {
            printf("bad parameter\n");
            return -1;
        }
        int utflen=wcstombs(NULL,unicodestr,0);
        if(utflen<=0)
        {
            printf("can not transfer!!!\n");
            return -1;
        }
        else if(utflen>=maxutfstrlen)
        {
            printf("dst str memory not enough\n");
            return -1;
        }
        wcstombs(utfstr,unicodestr,utflen);
        utfstr[utflen]=0;
        free(unicodestr);
        return utflen;
    }
    void Trim_redunt::make_page(const char* src_path,const char* des_path)//将文档内容提取出来并且转码
    {
        std::cout<<src_path<<std::endl;
        std::ifstream fin(src_path);
        std::ofstream fout;
        fout.open(des_path,std::ofstream::out | std::ofstream::app); 
        std::string line;
        std::string context;
        context.clear();
        int flag=0;
        int flag_out=0;
        while(getline(fin,line))
        {
            trim_space(line);//对文档内容进行去空格
            if(line.size()==0)//如果该行长度为0,则抛弃
            {
                continue;
            }
            std::istringstream sin(line);
            char* utfw=new char[1000]();
            std::string srcw;
            while(sin>>srcw)
            {
                std::stringstream ss;
                std::string temp;
                if(gbk2utf8(utfw,srcw.c_str(),1000)==-1)//如果转码失败直接抛弃该字符串
                {
                    flag=1;//直接退出循环
                    break;
                }
                ss<<utfw;
                ss>>temp;
                temp+=" ";
                context+=temp;
            }
            if(flag==1)
            {
                flag_out=1;     
                context.clear();  //直接抛弃该行
                break;
            }
            context+="\n";
        }
        if(flag_out==1)
        {
            return;//直接抛弃该文档
        }
        fout<<"<doc>"<<std::endl;
        fout<<"<docid>"<<++doc_num<<"</docid>"<<std::endl;
        fout<<"<url>"<<"http://www.google.com"<<"</url>"<<std::endl;
        fout<<"<content>"<<std::endl;
        int doc_id=doc_num;
        int offset=fout.tellp();
        int doc_len=context.size();
        std::cout<<"doc_id-->"<<doc_id<<" offset-->"<<offset<<" doc_len-->"<<doc_len<<std::endl;
        fout<<context;
        fout<<"</content>"<<std::endl;
        fout<<"</doc>"<<std::endl;
        std::cout<<src_path<<"is transfered over!"<<std::endl;
        std::cout<<"doc_id"<<doc_num<<"is put in page_lib"<<std::endl;
        fin.close();
        fout.close();
    }
    void Trim_redunt::make_page_lib(char* dir_path,char* des_path)
    {
        struct dirent* p_entry;
        struct stat my_stat;
        DIR* dir;
        memset(&my_stat,0,sizeof(my_stat));
        if(-1==lstat(dir_path,&my_stat))   //分析该目录状态
        {
            perror("lstat\n");
        }
        if(S_ISREG(my_stat.st_mode))//如果是普通文件，直接解析
        {
            make_page(dir_path,des_path);
        }
        else if(S_ISDIR(my_stat.st_mode))//如果是目录打开
        {
            dir=opendir(dir_path);
            if(dir==NULL)
            {
                perror("dir open failed\n");
            }
            while(memset(&my_stat,0,sizeof(my_stat)),(p_entry=readdir(dir))!=NULL)
            {
                if(strcmp(".",p_entry->d_name)==0||strcmp("..",p_entry->d_name)==0)
                    continue;
                char buf[1024];
                sprintf(buf,"%s/%s",dir_path,p_entry->d_name);//将目录绝对路径赋值buf
                make_page_lib(buf,des_path);
            }
        }
    }

}

