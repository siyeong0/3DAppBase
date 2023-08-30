#pragma once
#include <Windows.h>
#include <memory>
#include "Renderer.h"
#include "EDevice.h"

using std::unique_ptr;
using std::make_unique;

class ModelViewer;

extern ModelViewer* GetModelViewer();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ModelViewer
{
public:
	ModelViewer(eDevice device = eDevice::DX11);
	~ModelViewer();

	bool Initialize();
	void Run();

	Camera& GetCamera() const;
	RenderContext& GetModel();
	InputProc& GetInputProc();

	void ExitLoop();

private:
	eDevice meDev;

	unique_ptr<IDIRenderer> mRenderer;
	IDIGui* mGui;
	std::vector<IFrame*> mFrames;
	Camera* mMainCamera;
	InputProc mInputProc;
	RenderContext mMainModel;
	RenderContext mFloor;
	std::vector<RenderContext> mExtras;

	volatile bool mbLoopExit = false;

	constexpr static float FLOOR_Y = -1.0f;
};