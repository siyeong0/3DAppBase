#include "pch.h"
#include "IDIMeshObject.h"

IDIMeshObject::IDIMeshObject(const std::vector<MeshData> meshes)
{

}

IDIMeshObject::IDIMeshObject(const std::string& basePath, const std::string& fileName)
{

}

const Vector3& IDIMeshObject::GetExtents() const
{
	return mExtents;
}