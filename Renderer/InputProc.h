#pragma once
#include "dllAPI.h"
#include <functional>
#include <Windows.h>

// Key input : 0 ~ 255 (VK_...)
constexpr int MOUSE_MOVE = 256;
constexpr int MOUSE_LEFT = 257;
constexpr int MOUSE_RIGHT = 258;

class RENDERER_API InputProc
{
public:
	struct Act
	{
		std::function<void()> Down = nullptr;
		std::function<void()> Up = nullptr;
		std::function<void(float)> Keep = nullptr;
		bool bKeep = false;
	};
public:
	InputProc(int numActs = 1024);
	virtual ~InputProc();

	void Update(float dt);
	LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Act& operator[](int idx);

private:
	const int NUM_ACTS;
	std::vector<Act> mActs;
};