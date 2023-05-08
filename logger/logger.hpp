#include <string>

class Logger {
public:
  enum priority { FATAL = 0, ERR = 10, WARN = 20, INFO = 30, DEBUG = 40 };
  static Logger &getLogger();
  int prioritylevel = 0;

  Logger();
  // log with priority enum or int
  void Log(Logger::priority,  std::string format, ...);
  void Log(int priority);
};