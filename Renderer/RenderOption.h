#pragma once
#include "dllAPI.h"

enum class RENDERER_API eAntialiasing
{ 
	NONE,
	MSAA,
};

struct RENDERER_API ResolutionDesc
{
	int Width;
	int Height;
};

const ResolutionDesc _1280_720 = { 1280, 720 };
const ResolutionDesc _1920x1080 = { 1920, 1080 };
const ResolutionDesc _2560x1440 = { 2560, 1440 };

const ResolutionDesc DEFAULT_RESOLUTION = _1920x1080;

struct RENDERER_API RenderOption
{
	bool bFullScreen = false;
	ResolutionDesc Resolution = _1920x1080;

	eAntialiasing AA = eAntialiasing::MSAA;
	bool bWireFrame = false;
	float IBLStrength = 0.5f;
	
	float BloomStrength = 0.05f;
	float Exposure = 1.0f;
	float Gamma = 2.2f;

	bool bFrustumCulling = true;
	bool bDrawNormals = false;
};