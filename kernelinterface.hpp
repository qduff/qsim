#pragma once

#include "memdef.h"

// #include <cstddef>
#include <map>
#include <string>

#define CHARS_PER_LINE 30
#define VIDEO_LINES 16

class engineInterface {
private:
  void* allocateMemory(std::string name);
  bool spawnChild(std::string memname, std::string path);



public:
  engineInterface();
  ~engineInterface();

  int pid;
  memory_s *shmem;

  void writeAxis(float value, int id);
  void debugOsdPrint();

  bool start();
  bool isRunning;
  int wstatus;

  //
};