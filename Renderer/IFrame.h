#pragma once
#include "dllAPI.h"
#include "imgui.h"
#include <string>

class RENDERER_API IFrame
{
public:
	IFrame(int w, int h, double topLeftX, double topLeftY, double width, double height)
		: mTopLeftXRatio(topLeftX)
		, mTopLeftYRatio(topLeftY)
		, mWidthRatio(width)
		, mHeightRatio(height)
	{
		Resize(w, h);
	}

	virtual ~IFrame() = default;

	virtual void Draw() = 0;
	void Resize(int w, int h)
	{
		mTopLeftX = int(mTopLeftXRatio * w);
		mTopLeftY = int(mTopLeftYRatio * h);
		mWidth = int(mWidthRatio * w);
		mHeight = int(mHeightRatio * h);
	}
protected:
	double mTopLeftXRatio = 0;
	double mTopLeftYRatio = 0;
	double mWidthRatio = 0;
	double mHeightRatio = 0;
	int mTopLeftX = 0;
	int mTopLeftY = 0;
	int mWidth = 0;
	int mHeight = 0;
};