#pragma once
#include "dllAPI.h"
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

	void Resize(int w, int h);

	ImGuiContext* Context();
};