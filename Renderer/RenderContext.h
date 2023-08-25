#pragma once
#include "dllAPI.h"

#include <string>

#include "MathLib.h"

struct RENDERER_API RenderContext
{
	RenderContext(std::string name = "", Vector3 pos = Vector3(0,0,0))
		: MeshName(name)
		, TransformMatrix(Matrix::CreateTranslation(pos))
	{

	}
	~RenderContext() = default;

	std::string MeshName = "";
	Matrix TransformMatrix = Matrix();
};