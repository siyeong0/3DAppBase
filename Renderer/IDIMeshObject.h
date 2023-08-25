#pragma once
#include "dllAPI.h"
#include <vector>
#include <string>

#include "Geometry.h"
#include "ConstantBufferData.h"

class RENDERER_API IDIMeshObject
{
public:
	IDIMeshObject(const std::vector<MeshData> meshes);
	IDIMeshObject(const std::string& basePath, const std::string& fileName);
	virtual ~IDIMeshObject() = default;

	virtual void Initialize(const std::vector<MeshData>& meshes) = 0;
	virtual void Initialize(const std::string& basePath, const std::string& fileName) = 0;

	virtual void Render(const Matrix& tm) = 0;

	const Vector3& GetExtents() const;

public:
	MeshConstantsData mMeshConstData;
	MaterialConstantsData mMaterialConstData;
	// AABB Extents
	Vector3 mExtents;
};