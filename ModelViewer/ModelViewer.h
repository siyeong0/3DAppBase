#pragma once
#include <Windows.h>
#include <memory>
#include "Renderer.h"

using std::unique_ptr;
using std::make_unique;


class ModelViewer;

extern ModelViewer* GetModelViewer();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ModelViewer
{
public:
	ModelViewer();
	~ModelViewer() = default;

	bool Initialize();
	void Run();

	HWND GetWindow() const;
	Camera& GetCamera() const;
	RenderContext& GetModel();
	InputProc& GetInputProc();

	void ExitLoop();
private:
	unique_ptr<D3D11Renderer> mRenderer;
	Camera* mMainCamera;
	InputProc mInputProc;
	RenderContext mMainModel;
	RenderContext mFloor;
	std::vector<RenderContext> mExtras;

	volatile bool mbLoopExit = false;

	constexpr static float FLOOR_Y = -1.0f;
};