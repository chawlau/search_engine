#include "page_rank.h"
int main(int argc,char** argv)
{
    MY_CONF::CMyconf conf("../data/my.conf");
    //std::ofstream fout("page_cache");
    PAGE_RANK::Page_rank* rank=PAGE_RANK::Page_rank::getInstance(conf);
    //rank->get_page_index();
    /*
       std::vector<std::string> content;
       rank->get_content_by_doc_id(6531,content);
       std::cout<<content[0];
       std::cout<<content[1];
       std::cout<<content[2];
       return 0;*/
    //rank->get_doc_num();
    //rank->make_word_page_rindex("test_word_page_rindex");
    //rank->get_word_page_rindex();
    //rank->make_page_weight_rindex("test_page_weight_rindex");
    //rank->get_page_weight_rindex();
    //rank->make_page_module_index("page_module_index");
    //rank->get_page_module_index();
    /*rank->init();
    //std::string query;
    //std::ofstream fout("page_cache");*/
    Json::FastWriter writer;
    Json::Reader reader;
    /*
    std::ofstream fout("page_cache");
    Json::Value t_json;
    while(std::cout<<"请输入查询词汇"<<std::endl,std::cin>>query)
    {
        std::string result;
        hash_record title_content;
        if((*rank)(query,result,title_content))
        {
            Json::Value value1;
            Json::Value value;
            reader.parse(result,value);
            value1["search"]=query;
            value1["result"]=value;
            t_json.append(value1);
            for(int index=0;index!=value.size();index++)
            {
                std::cout<<value[index]["title"].asString()<<std::endl;
                std::cout<<value[index]["abstract"].asString()<<std::endl;
            }
            std::string order_title; 
            while(std::cout<<"请输入标题"<<std::endl,std::cin>>order_title)
            {
                if(order_title=="up")
                {
                    break;
                }
                if(title_content.count(order_title)>0)
                {
                    std::cout<<title_content[order_title]<<std::endl;
                }
                else
                {
                    std::cout<<"你查询的页面不存在或者已经被删除!"<<std::endl;
                }
            }
        }
        else 
        {
            Json::Value err_value;
            reader.parse(result,err_value,false);
            for(int index=0;index!=err_value.size();index++)
            {
                std::cout<<err_value[index]["title"].asString()<<std::endl;
                std::cout<<err_value[index]["abstract"].asString()<<std::endl;
            }
        }
    }
    std::string jsonfile=writer.write(t_json);
    fout<<jsonfile;
    fout.close();*/
    std::ifstream fin("page_cache");
    Json::Value jsvalue;
    reader.parse(fin,jsvalue,false);
    hash_record page_cache;
    for(int index=0;index!=jsvalue.size();index++)
    {
        std::string search=jsvalue[index]["search"].asString();
        Json::Value value5=jsvalue[index]["result"];
        std::string result=writer.write(value5);
        std::pair<std::string,std::string> apair=make_pair(search,result);
        page_cache.insert(apair);
    }
    piter iter=page_cache.begin();
    for(iter;iter!=page_cache.end();iter++)
    {
        std::cout<<iter->first<<std::endl;
        Json::Value value2;
        reader.parse(iter->second,value2,false);
        for(int index_i=0;index_i!=value2.size();index_i++)
        {
            std::cout<<value2[index_i]["title"].asString()<<std::endl;
            std::cout<<value2[index_i]["abstract"].asString()<<std::endl;
        }
    }
}
