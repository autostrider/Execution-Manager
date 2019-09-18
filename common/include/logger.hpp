#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>

namespace Logger
{
    struct LogHelper
    {
        ~LogHelper();
    };
    
    std::string getThreadId();
    std::string getCurrentTime();
    std::string getApplicationName(std::string);
}

#define LOG (Logger::LogHelper(), std::cout << Logger::getCurrentTime() \
                                  << " " \
                                  << Logger::getThreadId() \
                                  << " [ " << Logger::getApplicationName(std::string{__FILE__}) << " ]:\t")

#endif