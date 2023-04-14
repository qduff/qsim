#pragma once

#include "memdef.h"

// #include <cstddef>
#include <string>

#define CHARS_PER_LINE 30
#define VIDEO_LINES 16

class kernelInterface {
private:
  void* allocateMemory(std::string name);
  bool spawnChild(std::string memname, std::string path);



public:
  kernelInterface();
  ~kernelInterface();

  int pid;
  memory_s *shmem;

  void writeAxes(const float *axes, int count);
  void debugOsdPrint();

  bool start();
  bool isRunning;
  int wstatus;

  //
};