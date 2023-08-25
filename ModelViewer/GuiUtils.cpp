#include "GuiUtils.h"

#include <shlobj.h>
#include <iostream>
#include <sstream>

enum class eBrowse { FILE, FOLDER };

std::wstring toWString(const std::string& str)
{
	std::wstring wstr;
	wstr.assign(str.begin(), str.end());
	return wstr;
}

std::string toString(const std::wstring& wstr)
{
	std::string str;
	str.assign(wstr.begin(), wstr.end());
	return str;
}

static bool browse(std::wstring& outPath, std::wstring& outName, eBrowse type)
{
	HRESULT hr;
	bool br = false;

	size_t slash;
	// SIGDN flag;

	IFileDialog* pfd;
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (FAILED(hr)) goto RELEASE_EXIT_CO;

	if (type == eBrowse::FOLDER)
	{
		DWORD dwOptions;
		hr = pfd->GetOptions(&dwOptions);
		if (FAILED(hr)) goto RELEASE_EXIT_FD;
		pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
	}

	hr = pfd->Show(NULL);
	if (FAILED(hr)) goto RELEASE_EXIT_FD;

	IShellItem* psi;
	hr = pfd->GetResult(&psi);
	if (FAILED(hr)) goto RELEASE_EXIT_FD;

	PWSTR fpath;
	// flag = type == eBrowse::FILE ? SIGDN_FILESYSPATH : SIGDN_DESKTOPABSOLUTEPARSING;
	hr = psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &fpath);
	if (FAILED(hr)) goto RELEASE_EXIT_SI;

	outPath = std::wstring(fpath);
	slash = outPath.find_last_of(L"/\\");
	outName = outPath.substr(slash + 1);

	//  SUCCESS, CLEAN UP
	CoTaskMemFree(fpath);
	br = true;
RELEASE_EXIT_SI:
	psi->Release();
RELEASE_EXIT_FD:
	pfd->Release();
RELEASE_EXIT_CO:
	CoUninitialize();
	return br;
}

bool BrowseFile(std::wstring& outPath)
{
	std::wstring dummy;
	return BrowseFile(outPath, dummy);
}

// https://stackoverflow.com/questions/68601080/how-do-you-open-a-file-explorer-dialogue-in-c
bool BrowseFile(std::wstring& outPath, std::wstring& outName)
{
	return browse(outPath, outName, eBrowse::FILE);
}

bool BrowseFolder(std::wstring& outPath)
{
	std::wstring dummy;
	return BrowseFolder(outPath, dummy);
}

bool BrowseFolder(std::wstring& outPath, std::wstring& outName)
{
	return browse(outPath, outName, eBrowse::FOLDER);
}