#pragma once
#include "IDIGui.h"

namespace EditorGui
{
	void Assign(IDIGui* targetGui);

	void DrawTopMenuBar();
	void DrawLeftFrame();
	void DrawRightFrame();
	void DrawBottomFrame();

	int GetViewportWidth();
	int GetViewportHeight();
	int GetViewportTopLeftX();
	int GetViewportTopLeftY();
}	// EditorGui