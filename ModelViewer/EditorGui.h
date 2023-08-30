#pragma once
#include "IFrame.h"

namespace EditorGui
{
	class MenuBar : public IFrame
	{
	public:
		MenuBar();
		virtual ~MenuBar() = default;
		virtual void Draw() override;
	};

	class LeftFrame : public IFrame
	{
	public:
		LeftFrame();
		virtual ~LeftFrame() = default;
		virtual void Draw() override;
	};

	class RightFrame : public IFrame
	{
	public:
		RightFrame();
		virtual ~RightFrame() = default;
		virtual void Draw() override;
	};

	class BottomFrame : public IFrame
	{
	public:
		BottomFrame();
		virtual ~BottomFrame() = default;
		virtual void Draw() override;
	};

	int GetViewportWidth();
	int GetViewportHeight();
	int GetViewportTopLeftX();
	int GetViewportTopLeftY();
}	// EditorGui