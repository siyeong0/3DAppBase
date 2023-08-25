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
	IDIGui* GetGui() const;
	Camera& GetCamera() const;
	RenderContext& GetModel();
	InputProc& GetInputProc();
private:
	HWND mWindow;
	unique_ptr<D3D11Renderer> mRenderer;
	IDIGui* mGui;
	Camera* mMainCamera;
	InputProc mInputProc;
	RenderContext mMainModel;
	RenderContext mFloor;
	std::vector<RenderContext> mExtras;

	constexpr static float FLOOR_Y = -1.0f;
};