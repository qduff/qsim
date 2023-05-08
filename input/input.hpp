#pragma once

#include "../engineinterface.hpp"
#include <map>

template <typename T> T CLAMP(const T &value, const T &low, const T &high) {
  return value < low ? low : (value > high ? high : value);
}

class Input {
private:
  inline int *getSource(int i) { return &std::get<source>(rcmapping[i]); }
  inline float *getOffset(int i) { return &std::get<offset>(rcmapping[i]); }
  inline float *getDeadzone(int i) { return &std::get<deadzone>(rcmapping[i]); }
  inline float *getScale(int i) { return &std::get<scale>(rcmapping[i]); }

  inline float getAxisValue(int i) { return axesvalues[i]; }
  inline float applyDeadzone(int i, float val) {
    return (std::abs(val) > *getDeadzone(i)) ? val : 0.0f;
  }

  inline float getProcessed(int i) {
    return CLAMP(applyDeadzone(i, getAxisValue(*getSource(i))) * *getScale(i) +
                     *getOffset(i),
                 -1.0f, 1.0f);
  }

public:
  // std::map<int, std::tuple<int, float, float>> rcmapping =
  //     std::map<int, std::tuple<int, float, float>>{
  //         // DEST -> ORIGINAL, OFFSET, SCALING, //! add deadzon
  //         {0, {3, 0, 1}},
  //         {1, {4, 0, -1}},
  //         {2, {1, 0, -1}},
  //         {3, {0, 0, 1}},
  //     };

  enum mapping { source = 0, deadzone = 1, scale = 2, offset = 3 };

  std::map<int, std::tuple<int, float, float, float>> rcmapping =
      std::map<int, std::tuple<int, float, float, float>>{
          {0, {0, 0, 1, 0}},
          {1, {1, 0, 1, 0}},
          {2, {2, 0, 1, 0}},
          {3, {3, 0, 1, 0}},
      };

  bool present = false;
  const char *name;

  const float *axesvalues;
  int axescount = 0;

  const unsigned char *buttonvalues;
  int buttoncount = 0;

  void processAxes(engineInterface &ki);
  void renderOverlay();
};