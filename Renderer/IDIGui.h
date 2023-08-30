#pragma once
#include "dllAPI.h"
#include "IFrame.h"
#include <imgui.h>
#include <Windows.h>
#include <functional>
#include <string>

class IDIGui;

extern "C" RENDERER_API IDIGui * GetGui();

class RENDERER_API IDIGui
{
public:
	IDIGui();
	virtual ~IDIGui();

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;

	virtual void RenderBegin() = 0;
	void Render(const std::string name);
	virtual void RenderEnd() = 0;

	void Regist(const std::string name, IFrame* frame);
	void Resize(int w, int h);

	ImGuiContext* Context();

protected:
	std::unordered_map<std::string, IFrame*> mFrameTable;
};