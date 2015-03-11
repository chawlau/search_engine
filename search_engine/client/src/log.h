#ifndef __LOG_
#define __LOG_
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<pthread.h>
#include<errno.h>
#include<unistd.h>
#include"log4cpp/NDC.hh"
#include"log4cpp/Category.hh"
#include"log4cpp/Layout.hh"
#include"log4cpp/FileAppender.hh"
#include"log4cpp/OstreamAppender.hh"
#include"log4cpp/PatternLayout.hh"
#include "log4cpp/Priority.hh"
namespace LOG_SYSTEM
{
    struct Log_system
    {
        log4cpp::PatternLayout layout;
        log4cpp::Appender* file_app;
        log4cpp::Category& my_cat;
        Log_system(const std::string& log_file):layout(),my_cat(log4cpp::Category::getInstance("mycat"))
        {
            file_app=new log4cpp::FileAppender("fileapp",log_file.c_str());
        }
        void init();
        void* handle(void* arg);
        void debug(std::string msg);
        void emerg(std::string msg);
        void error(std::string msg);
        void warn(std::string msg);
        void crit(std::string msg);
        void info(std::string msg);
        void notice(std::string msg);
        void alert(std::string msg);
        void fatal(std::string msg);
    };
}
#endif
