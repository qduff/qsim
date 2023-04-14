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
#elif __MINGW32__
#else
#error unsupported!
#endif

int size = sizeof(memory_s);

// Remove shared memories
kernelInterface::~kernelInterface() {
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
#elif __MINGW32__
#endif
}

kernelInterface::kernelInterface() {}

// private

void *kernelInterface::allocateMemory(std::string name) {
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
  *shmem = memory_s{};
  if (shmem == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }
#elif __MINGW32__
#endif

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

bool kernelInterface::spawnChild(std::string memname, std::string path) {
#ifdef __linux__
  char *argv[] = {&path[0], &memname[0], NULL};
  int status;
  printf("spawn: %s %s\n", &path[0], &memname[0]);
  status = posix_spawn(&pid, &path[0], NULL, NULL, argv, environ);
#elif __MINGW32__
#endif
  return true;
}

// public

bool kernelInterface::start() {

  std::string memname = "name";
  allocateMemory(memname);

  std::string path(
      "/home/qduff/Documents/quad_sim_stuff/sim_kernel/build/sim_kernel");
  isRunning = spawnChild(memname, path);
  return isRunning;
}

void kernelInterface::writeAxes(const float *axes, int count) {
  // channel 1 - AIL, roll
  // channel 2 - ELE, pitc
  // channel 3 - THR, trottle
  // channel 4 - RUD, YAW
  auto mapping = std::map<int, std::tuple<int, float, float>>{
      // DEST -> ORIGINAL, OFFSET, SCALING
      {0, {3, 0, 1}},
      {1, {4, 0, -1}},
      {2, {1, 0, -1}},
      {3, {0, 0, 1}},
  };
  // POST MAPPING

  for (int i = 0; i < 16; ++i) {
    if (mapping.size() > i) {
      float val = axes[std::get<0>(mapping[i])] * std::get<2>(mapping[i]);
      shmem->rc[i] = val;
      // printf("SENDOUT %i :  %f\n", i, shmem->rc[i]);
    } else {
      shmem->rc[i] = 0;
    }
  }
  // #endif
}

void kernelInterface::debugOsdPrint() {
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
