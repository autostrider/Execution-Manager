#include "logger.hpp"

#include <ctime> 
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

namespace
{
    const std::string appPath = std::string{"../applications/"};
}
namespace  Logger
{
    LogHelper::~LogHelper()
    { 
        std::cout << std::endl;
    }
    
    std::string  getThreadId()
    {
        std::stringstream ss;
        ss << std::hex << "0x" << std::this_thread::get_id();
        
        return ss.str();
    }

    std::string getCurrentTime()
    {
        auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%X");
    
        return ss.str();
    }
    
    std::string getApplicationName(std::string filePath)
    {        
        filePath = filePath.erase(0, appPath.size());

        return filePath.erase(filePath.find_first_of('/'), std::string::npos);
    }
    
}