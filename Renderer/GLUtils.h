#pragma once
#include "dllAPI.h"
#include <GL/glew.h>

class RENDERER_API GLUtils
{
public:
	static GLuint CreateProgram(const char* vertexPath, const char* pixelPath);
};