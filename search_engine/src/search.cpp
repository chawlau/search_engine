#include"search.h"
namespace WORD_CORRECT
{
    bool CMypair::operator<(const CMypair& right) const
    {
        if(m_dis==right.m_dis)
        {
            return m_freq>right.m_freq;
        }
        else 
        {
            return m_dis<right.m_dis;
        }
    }
    void CMypair::pair_out()
    {
        std::cout<<"<"<<m_str<<" "<<m_freq<<">"<<" :"<<m_dis<<"-->";
    }
    CWord_correct::CWord_correct(const char*  path):dic_path(path),r_index(HASH_BASE)
    {
        make_index(r_index,dic_path);
        std::cout<<"r_index has established!"<<std::endl;
    }
    void CWord_correct::index_out(strhash& r_index)
    {
        striter iter=r_index.begin();
        for(iter;iter!=r_index.end();iter++)
        {
            std::cout<<iter->first<<"  ";
            std::set<std::pair<std::string,int> >::iterator iter1=iter->second.begin();
            for(iter1;iter1!=iter->second.end();iter1++)
            {
                std::cout<<"<"<<iter1->first<<" "<<iter1->second<<">";
            }
            std::cout<<std::endl;
        }
    }
    void CWord_correct::build(const std::string& str,const int& cnt,strhash& r_index)
    {
        for(size_t index=0;index!=str.size();)
        {
            if(str[index]>0)
            {
                std::string temp=str.substr(index,1);
                std::pair<std::string,int> apair=make_pair(str,cnt);
                r_index[temp].insert(apair);
                index++;
            }
            else if((str[index]&0xF0)==0xC0)
            {
                std::string temp=str.substr(index,2);
                std::pair<std::string,int> apair=make_pair(str,cnt);
                r_index[temp].insert(apair);
                index+=2;
            }
            else if((str[index]&0xF0)==0xE0)
            {
                std::string temp=str.substr(index,3);
                std::pair<std::string,int> apair=make_pair(str,cnt);
                r_index[temp].insert(apair);
                index+=3;
            }
            else
            {
                return;
            }
        }
    }
    void CWord_correct::split_str(const std::string& str,std::vector<std::string>& dest)
    {
        for(size_t index=0;index!=str.size();)
        {
            if(str[index]>0)
            {
                std::string temp=str.substr(index,1);
                dest.push_back(temp);
                index++;
            }
            else if((str[index]&0xF0)==0xC0)
            {
                std::string temp=str.substr(index,2);
                dest.push_back(temp);
                index+=2;
            }
            else if((str[index]&0xF0)==0xE0)
            {
                std::string temp=str.substr(index,3);
                dest.push_back(temp);
                index+=3;
            }
            else
            {
                return;
            }
        }
    }
    void CWord_correct::make_index(strhash& r_index,const char* dic_path)
    {
        std::fstream fin(dic_path);
        std::string line;
        while(getline(fin,line))
        {
            std::istringstream ss(line);
            std::string temp;
            int cnt=0;
            while(ss>>temp>>cnt)
            {
                build(temp,cnt,r_index);
            }
        }
    }
    void CWord_correct::make_cache(word_cache& cache,strhash& r_index,const std::string& search_word)
    {
        std::vector<std::string> ivec;
        split_str(search_word,ivec);//将查询词汇分割成每个单字,然后存到ivec中
        std::vector<std::string>::iterator iter=ivec.begin();
        for(iter;iter!=ivec.end();iter++)
        {
            std::set<std::pair<std::string,int> > temp=r_index[*iter];
            std::set<std::pair<std::string,int> >::iterator iter=temp.begin();
            for(iter;iter!=temp.end();iter++)
            {
                std::pair<std::string,int> apair=make_pair(iter->first,iter->second);
                cache.insert(apair);    
            }
        }
    }
    void CWord_correct::cache_out(word_cache& cache)
    {
        word_cache_iter iter=cache.begin();
        for(iter;iter!=cache.end();iter++)
        {
            std::cout<<"<"<<iter->first<<" "<<iter->second<<">"<<"-->";    
        }
    }
    void CWord_correct::get_correct(word_cache& cache,std::vector<CMypair>& re_vec,const std::string& search_word)
    {
        word_cache_iter iter=cache.begin();
        std::priority_queue<CMypair> top10;
        for(iter;iter!=cache.end();iter++)
        {
            CMypair* apair=new CMypair(iter->first,iter->second,edit(search_word,iter->first));
            if(top10.size()<10)
            {
                top10.push(*apair);
            }
            else
            {
                if(top10.top()<*apair)
                {
                    continue;
                }
                else
                {
                    top10.pop();
                    top10.push(*apair);
                }
            }
        }
        while(top10.size())
        {
            re_vec.push_back(top10.top());
            top10.pop();
        }
    }
    void  CWord_correct::operator()(const std::string& search_word,std::string& search_result)
    {
        word_cache cache;
        std::vector<CMypair> re_vec;
        make_cache(cache,r_index,search_word);
        Json::Value correct_value;
        Json::FastWriter writer;
        if(cache.count(search_word)>0)
        {
            correct_value["type"]="输入正确";
            Json::Value value;
            Json::Value temp;
            value["result"]="输入正确";
            temp.append(value);
            correct_value["result"]=temp;
        }
        else
        {
            correct_value["type"]="输入有误,以下候选词可能是你希望输入的";
            get_correct(cache,re_vec,search_word);
            Json::Value value;
            if(re_vec.size())
            {
                std::vector<CMypair>::reverse_iterator r_iter=re_vec.rbegin();
                for(r_iter;r_iter!=re_vec.rend();r_iter++)
                {
                    //std::cout<<r_iter->m_str<<"->freq: "<<r_iter->m_freq<<"->m_dis: "<<r_iter->m_dis<<std::endl;
                    Json::Value temp;
                    temp["result"]=r_iter->m_str;
                    value.append(temp);
                }
                correct_value["result"]=value;
            }
            else
            {
                Json::Value value;
                Json::Value temp;
                value["result"]="无此纠错结果,请输入其他查询词汇";
                temp.append(value);
                correct_value["result"]=temp;
            }
        }
        search_result=writer.write(correct_value);
    }
}
