#pragma once

#include "memdef.h"

// #include <cstddef>
#include <map>
#include <string>
#include "memdef.h"
#include <cassert>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <string.h>

#ifdef __linux__
#include <cstdio>
#include <fcntl.h>
#include <spawn.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ucontext.h>
#include <sys/wait.h>
#include <unistd.h>
#elif defined _WIN32 || defined __CYGWIN__
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#else
#error unsupported!
#endif

#define CHARS_PER_LINE 30
#define VIDEO_LINES 16

class engineInterface {
private:
  void* allocateSharedMemory(std::string name);
  bool spawnChild(std::string memname, std::string path);



public:
  engineInterface();
  ~engineInterface();

  memory_s *shmem;

  void writeAxis(float value, int id);
  void debugOsdPrint();

  bool start();

  bool checkRunning();
  bool isRunning;

#ifdef __linux__
  int pid;
  int wstatus;
#elif defined _WIN32  || defined __CYGWIN__
  STARTUPINFO si = {sizeof(si)}; // not really that useful!
  PROCESS_INFORMATION pi;
  HANDLE hMapFile;
#endif
};