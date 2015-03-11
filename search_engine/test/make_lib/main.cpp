#include "make_lib.h"
int main(int argc,char** argv)
{
    MY_CONF::CMyconf conf("../data/my.conf");
    TRIM_REDUNT::Trim_redunt* trim=TRIM_REDUNT::Trim_redunt::getInstance(conf);
    //trim->make_page_lib("../data/source","test_page_lib");
    //trim->make_page_content_index("content_index");
    //trim->make_page_index("content_index");
    //trim->get_page_content_index();
    trim->get_page_index();
    trim->get_doc_num();
    std::vector<std::string> content;
    trim->get_content_by_doc_id(6531,content);
    std::cout<<"title: "<<content[0];
    std::cout<<"abstract: "<<content[1];
    std::cout<<"content:  "<<content[2];
    //trim->make_page_title_abstract();
    //trim->remv_dup("new_page_lib");
}
