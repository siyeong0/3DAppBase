#pragma once
#include "pch.h"
#include "IDIMeshObject.h"

#include "D3D11Mesh.h"
#include "D3D11Utils.h"
#include "D3D11Common.h"
#include "D3D11PSO.h"

#include <memory>

class RENDERER_API D3D11MeshObject : public IDIMeshObject
{
public:
	static void Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);
public:
	D3D11MeshObject(const std::vector<MeshData>& meshes);
	D3D11MeshObject(const std::string& basePath, const std::string& fileName);
	virtual ~D3D11MeshObject();

	virtual void Initialize(const std::vector<MeshData>& meshes) override;
	virtual void Initialize(const std::string& basePath, const std::string& filename) override;

	virtual void Render(const Matrix& tm) override;

private:
	std::vector<D3D11Mesh*> mMeshes;

	ComPtr<ID3D11Buffer> mMeshConstBuffer;
	ComPtr<ID3D11Buffer> mMaterialConstBuffer;

	ComPtr<ID3D11Buffer> mDrawNormalConstantBuffer;
	ComPtr<ID3D11Buffer> mNormalPixelConstantBuffer;
};