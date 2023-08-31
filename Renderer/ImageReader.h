#pragma once
#include "dllAPI.h"
#include <string>
#include <vector>

extern "C" RENDERER_API void ReadImage(const std::string filename, std::vector<uint8_t>&image, int& width, int& height);
