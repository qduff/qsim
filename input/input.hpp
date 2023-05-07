#pragma once

#include "../kernelinterface.hpp"
#include <map>

class Input {
public:
  // std::map<int, std::tuple<int, float, float>> rcmapping =
  //     std::map<int, std::tuple<int, float, float>>{
  //         // DEST -> ORIGINAL, OFFSET, SCALING, //! add deadzon
  //         {0, {3, 0, 1}},
  //         {1, {4, 0, -1}},
  //         {2, {1, 0, -1}},
  //         {3, {0, 0, 1}},
  //     };

  std::map<int, std::tuple<int, float, float>> rcmapping =
      std::map<int, std::tuple<int, float, float>>{
          // DEST -> ORIGINAL, OFFSET, SCALING, //! add deadzon
          {0, {0, 0, 1}},
          {1, {1, 0, 1}},
          {2, {2, 0, 1}},
          {3, {3, 0, 1}},
      };

  const float *lastaxes;
  int lastcount = 0;
  bool present = false;

  void processAxes(engineInterface &ki, const char *name, const float *axes,
                   int count);
  void renderOverlay();
};