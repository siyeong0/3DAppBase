#pragma once
#include <string>

#pragma warning(disable : 4244)

std::wstring toWString(const std::string& str);
std::string toString(const std::wstring& wstr);

bool BrowseFile(std::wstring& outPath);
bool BrowseFile(std::wstring& outPath, std::wstring& outName);
bool BrowseFolder(std::wstring& outPath);
bool BrowseFolder(std::wstring& outPath, std::wstring& outName);