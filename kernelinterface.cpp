#include "kernelinterface.hpp"
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
#elif __MINGW32__ || defined __CYGWIN__
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
STARTUPINFO si={sizeof(si)};
PROCESS_INFORMATION pi;
HANDLE hMapFile;
#else
#error unsupported!
#endif

int size = sizeof(memory_s);

// Remove shared memories
engineInterface::~engineInterface() {
#ifdef __linux__
  kill(pid, SIGSEGV);
  int res = munmap(shmem, size);
  if (res == -1) {
    perror("munmap");
  }
  int fd = shm_unlink("name");
  if (fd == -1) {
    perror("unlink");
  }
#elif __MINGW32__ || defined __CYGWIN__
  puts("killin");
  TerminateProcess(pi.hProcess, 1);
   UnmapViewOfFile(shmem); // ! mustbestored!
   CloseHandle(hMapFile);
  CloseHandle( pi.hProcess );
  CloseHandle( pi.hThread );
puts("KILLED SUCCESSFULY >:)");
#endif
}

engineInterface::engineInterface() {}

// private

void *engineInterface::allocateMemory(std::string name) {
#ifdef __linux__
  int fd = shm_open(name.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("openm");
    return NULL;
  }
  int res = ftruncate(fd, size);
  if (res == -1) {
    perror("ftruncate");
    return NULL;
  }
  shmem = (memory_s *)mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmem == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }
#elif __MINGW32__ || defined __CYGWIN__
LPCSTR mname = name.c_str();
  
   hMapFile = CreateFileMapping(
                 INVALID_HANDLE_VALUE,    // use paging file
                 NULL,                    // default security
                 PAGE_READWRITE,          // read/write access
                 0,                       // maximum object size (high-order DWORD)
                 sizeof(memory_s),                // maximum object size (low-order DWORD)
                 mname);                 // name of mapping object
   if (hMapFile == NULL)
   {
      printf("Could not create file mapping object (%d).\n",
             GetLastError());
      return NULL;
   }
   shmem = (memory_s *)MapViewOfFile(hMapFile,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        sizeof(memory_s));
    printf("%p\n\n",shmem);
   if (shmem == NULL)
   {
      printf("Could not map view of file (%d).\n",
             GetLastError());
       CloseHandle(hMapFile);
      return NULL;
   }
#endif
  *shmem = memory_s{};

  //! MOVE THIS STUFF!
  shmem->parentVersion[0] = 0;
  shmem->parentVersion[1] = 0;
  shmem->parentVersion[2] = 2;

  if (shmem->schemaVersion[0] != 0) {
    fprintf(stderr, "UNSUPOROTED VER!");
    exit(-1);
  }

  return shmem;
}

bool engineInterface::spawnChild(std::string memname, std::string path) {
  printf("spawn: %s %s\n", &path[0], &memname[0]);
#ifdef __linux__
  char *argv[] = {&path[0], &memname[0], NULL};
  int status;
  status = posix_spawn(&pid, &path[0], NULL, NULL, argv, environ);
#elif __MINGW32__ || defined __CYGWIN__
  if (! CreateProcess( path.c_str(),
  ("qsim_engine " + memname).data(),
  // such that memname = argv[1] and not 0 on windoez
  // might be worth fixing on engine side doe
  NULL, NULL, TRUE, 0, NULL, NULL , &si, &pi
  )) // suspiciously reasonable for windows
  {
    printf( "CreateProcess failed (%d).\n", GetLastError() );
    return false;
  }
#endif
  return true;
}

// public

bool engineInterface::start() {

  std::string memname = "name";
  allocateMemory(memname);

  #if defined __MINGW32__ || defined __CYGWIN__
  std::string path(
      "..\\..\\qsim-engine-betaflight\\build\\qsim_engine.exe");
  #else
  std::string path(
      "/home/qduff/Documents/quad_sim_stuff/sim_kernel/build/qsim_engine");
  #endif
  isRunning = spawnChild(memname, path);
  return isRunning;
}

void engineInterface::writeAxis(float value, int id) {
  // channel 1 - AIL, roll
  // channel 2 - ELE, pitc
  // channel 3 - THR, trottle
  // channel 4 - RUD, YAW
  
  // POST MAPPING
    shmem->rc[id] = value;
}

void engineInterface::debugOsdPrint() {
  printf("\033[H\033[J");
  for (size_t i = 0; i < VIDEO_LINES * CHARS_PER_LINE; ++i) {
    if (i % CHARS_PER_LINE == 0) {
      printf("\n");
    }
    if (shmem->osd[i] == 0) {
      printf(" ");
    } else {
      printf("%c", shmem->osd[i]);
    }
  }
  puts("\n------------------------------");
}
