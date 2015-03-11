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
        std::string EU="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::string EL="abcdefghijklmnopqrstuvwxyz0123456789";
        int start=0;
        while((start=line.find_first_of(EU,start))!=std::string::npos)
        {
            line[start]+=32;
        }
        start=0;
        while((start=line.find_first_not_of(EL,start))!=std::string::npos)
        {
            line.replace(start,1,1,' ');
            start++;
        }
        std::istringstream sin(line);
        std::string word;
        while(sin>>word)
        {
            word_count[word]++;
        }
    }
    fin1.close();
    std::ofstream fout;
    fout.open(out_path,std::ofstream::out | std::ofstream::app);
    piter iter=word_count.begin();
    while(iter!=word_count.end())
    {
        //fout<<iter->first<<std::setw(10)<<str2int(iter->first)<<std::setw(10)<<iter->second<<std::setw(10)<<std::endl;
        fout<<iter->first<<std::setw(10)<<iter->second<<std::setw(10)<<std::endl;
        iter++;
    }
    fout.close();
}
int main(int argc,char* argv[])
{
    struct dirent* p_entry;
    DIR* dir;
    dir=opendir(argv[1]);
    if(dir==NULL)
    {
        perror("dir open failed\n");
    }
    std::stringstream src_path;
    while((p_entry=readdir(dir))!=NULL)
    {
        if(strcmp(".",p_entry->d_name)==0||strcmp("..",p_entry->d_name)==0)
            continue;
        src_path<<argv[1]<<"/"<<p_entry->d_name;
        extract(src_path.str().c_str(),argv[2]);
        std::cout<<p_entry->d_name<<"'s word extracting task is over"<<std::endl;
    }
}
