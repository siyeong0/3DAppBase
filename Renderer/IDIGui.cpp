#include "pch.h"
#include "IDIGui.h"

IDIGui::IDIGui()
{

}

IDIGui::~IDIGui()
{

}

ImGuiContext* IDIGui::Context()
{
	return ImGui::GetCurrentContext();
}

void IDIGui::Resize(int w, int h)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.DisplaySize = ImVec2(float(w), float(h));
}