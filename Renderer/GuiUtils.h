#pragma once
#include "dllAPI.h"
#include <Windows.h>
#include <string>

extern "C" RENDERER_API bool BrowseFile(std::wstring & outPath, std::wstring & outName);

extern "C" RENDERER_API bool BrowseFolder(std::wstring& outPath, std::wstring& outName);