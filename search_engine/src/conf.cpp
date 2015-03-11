#include "conf.h"
namespace MY_CONF
{
    CMyconf::CMyconf(const std::string& conf_path)
    {
        std::fstream fin(conf_path.c_str());
        std::string line;
        int cnt;
        while(getline(fin,line))
        {
            std::istringstream sin(line);
            std::string name,data;
            while(sin>>name>>data)
            {
                config.insert(make_pair(name,data));
            }
        }
        std::cout<<"config init succeed!"<<std::endl;
        fin.clear();
    }
    void CMyconf::conf_show()
    {
        std::cout<<"my_ip:"<<config["my_ip"]<<std::endl;  
        std::cout<<"my_port:"<<atoi(config["my_port"].c_str())<<std::endl;  
        std::cout<<"my_dic:"<<config["my_dic"]<<std::endl;  
        std::cout<<"my_cache:"<<config["my_cache"]<<std::endl;  
        std::cout<<"my_log:"<<config["my_log"]<<std::endl;  
    }
}
