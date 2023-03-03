#include "kernelinterface.hpp"
#include <cassert>
#include <cstdint>

kernelInterface::~kernelInterface() {}

kernelInterface::kernelInterface() {
  std::string memname = "shrdx";

  boost::interprocess::shared_memory_object::remove(memname.c_str());
  shm = {boost::interprocess::create_only, memname.c_str(), 65535};

  osdpnt = shm.construct<uint8_t>("osdpnt")[VIDEO_LINES * CHARS_PER_LINE](' ');
  rcpnt = shm.construct<float>("rcpnt")[8](0);
}

bool kernelInterface::isRunning() { return c.running(); }

bool kernelInterface::start() {
  c = boost::process::child(
      "/home/qduff/Documents/quad_sim_stuff/sim_kernel/build/sim_kernel");
  // boost::process::std_out > out);

  printf("started!\n");
  if (!c.running()) {
    printf("notrun!\n");
    return false;
  }
  return true;
}

void kernelInterface::writeAxes(const float *axes, int count) {
  assert(count < 8);
  for (int i = 0; i < count; ++i) {
    *(rcpnt + i) = axes[i];
    // printf("axis %i :  %f\n", i, *(rcpnt + i));
  }
}

void kernelInterface::debugOsdPrint() {
  printf("\033[H\033[J");
  for (size_t i = 0; i < VIDEO_LINES * CHARS_PER_LINE; ++i) {
    if (i % CHARS_PER_LINE == 0) {
      printf("\n");
    }
    if (*(osdpnt + i) == 0) {
      printf(" ");
    } else {
      printf("%c", *(osdpnt + i));
    }
  }
  puts("\n------------------------------");
}