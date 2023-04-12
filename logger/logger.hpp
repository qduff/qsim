#include <string>

class Logger {
public:
  enum priority { FATAL = 10, ERROR = 30, WARN = 20, INFO = 10, DEBUG = 0 };
  static Logger &getLogger();
  int prioritylevel = 0;

  // log with priority enum or int
  void Log(Logger::priority,  std::string format, ...);
  void Log(int priority);
};