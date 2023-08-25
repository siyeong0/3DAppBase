#include "pch.h"
#include "IDIGui.h"

IDIGui::IDIGui()
	: TargetRenderer(nullptr)
{

}

IDIGui::~IDIGui()
{

}

ImGuiContext* IDIGui::Context()
{
	return ImGui::GetCurrentContext();
}

void IDIGui::Assgin(IDIRenderer* targetRenderer)
{
	TargetRenderer = targetRenderer;
}

LRESULT IDIGui::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.DisplaySize = ImVec2(float(int(LOWORD(lParam))), float(int(HIWORD(lParam))));
		break;
	}

	return 0;
}