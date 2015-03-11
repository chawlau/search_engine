#ifndef __CONF_
#define __CONF_
#include<map>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<cstring>
namespace MY_CONF
{
    struct CMyconf
    {
        std::map<std::string,std::string> config;
        CMyconf(const std::string& conf_path);
        void conf_show();
    };
}
#endif
