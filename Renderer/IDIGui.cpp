#include "pch.h"
#include "IDIGui.h"

IDIGui* gGui = nullptr;

IDIGui* GetGui()
{
	return gGui;
}

IDIGui::IDIGui()
{
	gGui = this;
}

IDIGui::~IDIGui()
{
	gGui = nullptr;
}

void IDIGui::Render(const std::string name)
{
	mFrameTable[name]->Draw();
}

void IDIGui::Regist(const std::string name, IFrame* frame)
{
	mFrameTable[name] = frame;
}

void IDIGui::Resize(int w, int h)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.DisplaySize = ImVec2(float(w), float(h));

	for (auto& f : mFrameTable)
	{
		f.second->Resize(w,h);
	}
}

ImGuiContext* IDIGui::Context()
{
	return ImGui::GetCurrentContext();
}