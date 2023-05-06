#include "logger.hpp"
#include <iostream>
#include <vector>
#include <cstdarg>

#if defined __MINGW32__ || defined __CYGWIN__
#include <stdarg.h>
#endif


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

    std::vfprintf(stdout, zcFormat, vaArgs);
    va_end(vaArgs);

    // https://stackoverflow.com/questions/19009094/c-variable-arguments-with-stdstring-only
  }
}
