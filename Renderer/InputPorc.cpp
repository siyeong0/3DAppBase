#include "pch.h"
#include "InputProc.h"

InputProc::InputProc(int numActs)
	:NUM_ACTS(numActs)
{
	mActs.reserve(NUM_ACTS);
	for (int i = 0; i < NUM_ACTS; i++)
	{
		mActs.push_back(Act());
	}
}
InputProc::~InputProc()
{

}

InputProc::Act& InputProc::operator[](int idx)
{
	return mActs[idx];
}

void InputProc::Update(float dt)
{
	for (auto& act : mActs)
	{
		if (act.Keep != nullptr && act.bKeep == true)
		{
			act.Keep(dt);
		}
	}
}

LRESULT InputProc::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		mActs[MOUSE_LEFT].bKeep = true;
		if (mActs[MOUSE_LEFT].Down != nullptr)
		{
			mActs[MOUSE_LEFT].Down();
		}
		break;
	case WM_LBUTTONUP:
		mActs[MOUSE_LEFT].bKeep = false;
		if (mActs[MOUSE_LEFT].Up != nullptr)
		{
			mActs[MOUSE_LEFT].Up();
		}
		break;
	case WM_RBUTTONDOWN:
		mActs[MOUSE_RIGHT].bKeep = true;
		if (mActs[MOUSE_RIGHT].Down != nullptr)
		{
			mActs[MOUSE_RIGHT].Down();
		}
		break;
	case WM_RBUTTONUP:
		mActs[MOUSE_RIGHT].bKeep = false;
		if (mActs[MOUSE_RIGHT].Up != nullptr)
		{
			mActs[MOUSE_RIGHT].Up();
		}
		break;
	case WM_KEYDOWN:
		mActs[int(wParam)].bKeep = true;
		if (mActs[int(wParam)].Down != nullptr)
		{
			mActs[int(wParam)].Down();
		}
		break;
	case WM_KEYUP:
		mActs[int(wParam)].bKeep = false;
		if (mActs[int(wParam)].Up != nullptr)
		{
			mActs[int(wParam)].Up();
		}
		break;
	}

	return 0;
}