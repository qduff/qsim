#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/process.hpp>

#define CHARS_PER_LINE 30
#define VIDEO_LINES 16

class kernelInterface {
private:
  boost::interprocess::managed_shared_memory shm;
  // boost::process::ipstream out;
  boost::process::child c;
  std::string memname;

public:
  kernelInterface();
  ~kernelInterface();

  uint8_t *osdpnt;
  float *rcpnt;

  void writeAxes(const float *axes, int count);
  void debugOsdPrint();

  bool start();
  bool isRunning();

  //
};