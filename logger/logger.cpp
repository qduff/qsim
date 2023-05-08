#include "logger.hpp"
#include <cstdio>
#include <iostream>
#include <vector>
#include <cstdarg>
#include <ctime>

#if defined _WIN32 || defined __CYGWIN__
#include <stdarg.h>
#endif

Logger::Logger(){
}

Logger &Logger::getLogger() {
  static Logger logger;
  return logger;
}

void Logger::Log(Logger::priority priority, std::string format, ...) {
  if (priority >= prioritylevel) {
    const char *const zcFormat = format.c_str();

    va_list vaArgs;
    va_start(vaArgs, format);

    va_list vaCopy;
    va_copy(vaCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaCopy);
    va_end(vaCopy);

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
    std::string str(buffer);
    std::cout << str;

    std::fprintf(stdout, " %i ", priority);
    std::vfprintf(stdout, zcFormat, vaArgs);
    va_end(vaArgs);
    std::cout << std::endl;


    // https://stackoverflow.com/questions/19009094/c-variable-arguments-with-stdstring-only
  }
}
