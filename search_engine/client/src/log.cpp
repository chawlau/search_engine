#include "log.h"
namespace LOG_SYSTEM
{
    void Log_system::init()
    {
        layout.setConversionPattern("%d{%Y-%m-%d %H:%M:%S,%l}: %p %m%n");
        file_app->setLayout(&layout);
        my_cat.setPriority(log4cpp::Priority::DEBUG);
        my_cat.addAppender(file_app);
        std::cout<<"log_system init succeed!"<<std::endl;
    }
    void Log_system::notice(std::string msg)
    {
        my_cat.notice(msg);
    }
    void Log_system::info(std::string msg)
    {
        my_cat.info(msg);
    }
    void Log_system::crit(std::string msg)
    {
        my_cat.crit(msg);
    }
    void Log_system::warn(std::string msg)
    {
        my_cat.warn(msg);
    }
    void Log_system::error(std::string msg)
    {
        my_cat.error(msg);
    }
    void Log_system::fatal(std::string msg)
    {
        my_cat.fatal(msg);
    }
    void Log_system::emerg(std::string msg)
    {
        my_cat.emerg(msg);
    }
    void Log_system::debug(std::string msg)
    {
        my_cat.debug(msg);
    }
    void Log_system::alert(std::string msg)
    {
        my_cat.info(msg);
    }
}
