#pragma once
#include "dllAPI.h"
#include "IDIRenderer.h"
#include <imgui.h>
#include <Windows.h>

class RENDERER_API IDIGui
{
public:
	IDIGui();
	virtual ~IDIGui();

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;

	virtual void RenderBegin() = 0;
	virtual void RenderEnd() = 0;

	void Assgin(IDIRenderer* targetRenderer);
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	ImGuiContext* Context();

public:
	IDIRenderer* TargetRenderer;
};