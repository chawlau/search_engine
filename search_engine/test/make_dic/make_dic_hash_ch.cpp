#include<iostream>
#include<fstream>
#include<sstream>
#include<set>
#include<map>
#include<utility>
#include<iomanip>
#include<sys/types.h>
#include<dirent.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/dir.h>
#include<cstring>
#include<string>
#include<cctype>
#include<cstdio>
#include<cstdlib>
#include<ext/hash_map>
namespace NM
{
    struct CMyhash
    {
        int operator()(const std::string& obj) const
        {
            __gnu_cxx::hash<const char*> chash;
            return static_cast<int>(chash(obj.c_str()));
        }
    };
}
int str2int(const std::string str)
{
    int data=0;
    for(int i=0;i!=str.size();i++)
    {
        data+=str[i];
    }
    return data;
}
int isch(std::string str)
{
    for(int index=0;index!=str.size();index++)
    {
        if(isalnum(str[index])||ispunct(str[index]))
        {
            return 0;
        }
    }
    return 1;
}
std::string& find_replace(std::string& str_line, const std::string& str1 , const std::string& str2)
{
    std::string::size_type pos;
    size_t CPsize = str1.size();
    pos = str_line.find(str1); // 查找 str1 替换成 str2
    while (pos != std::string::npos) 
    {
            str_line.replace(pos, CPsize, str2);
            pos = str_line.find(str1, pos + 1);
    }
    return str_line;
}
std::string& ch_to_en(std::string& str_line)
{
    find_replace(str_line, "“", "\"");
    find_replace(str_line, "”", "\"");
    find_replace(str_line, "‘", "\'");
    find_replace(str_line, "’", "\'");
    find_replace(str_line, "，", ",");
    find_replace(str_line, "。", ".");
    find_replace(str_line, "！", "!");
    find_replace(str_line, "？", "?");
    find_replace(str_line, "；", ";");
    find_replace(str_line, "：", ":");
    find_replace(str_line, "　", " "); // 全角空格
    find_replace(str_line, "（", "("); //
    find_replace(str_line, "）", ")");
    find_replace(str_line, "〈", "<");
    find_replace(str_line, "〉", ">");
    find_replace(str_line, "、", " ");
    return str_line;
}
void extract(const char* source_path,const char* out_path)
{
    typedef __gnu_cxx::hash_map<std::string,int,NM::CMyhash> strhash;
    typedef __gnu_cxx::hash_map<std::string,int,NM::CMyhash>::iterator piter;
    strhash word_count(10000);
    std::pair<piter,bool> pair_ok;
    std::ifstream fin1(source_path);
    std::string line;
    while(getline(fin1,line))
    {
        std::istringstream sin(line);
        std::string word;
        while(sin>>word)
        {
            ch_to_en(word);
            if(isch(word))
            {
                word_count[word]++;
            }
            else
            {
                continue;
            }
        }
    }
    fin1.close();
    std::ofstream fout;
    fout.open(out_path,std::ofstream::out | std::ofstream::app);
    piter iter=word_count.begin();
    while(iter!=word_count.end())
    {
        fout<<iter->first<<std::setw(10)<<iter->second<<std::setw(10)<<std::endl;
        iter++;
    }
    fout.close();
}
int main(int argc,char* argv[])
{
    extract(argv[1],argv[2]);
    std::cout<<"divide task is over!"<<std::endl;
}
