#include<iostream>
int main(int argc,char** argv)
{
    std::string word;
    while(std::cout<<"please input"<<std::endl,std::cin>>word)
    {
        std::cout<<word<<std::endl;
    }
    std::cout<<"over"<<std::endl;
}
