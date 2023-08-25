#include "pch.h"
#include "D3D11MeshObject.h"

static ComPtr<ID3D11Device> mDevice;
static ComPtr<ID3D11DeviceContext> mContext;

void D3D11MeshObject::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	mDevice = device;
	mContext = context;
}

D3D11MeshObject::D3D11MeshObject(const std::vector<MeshData>& meshes)
	: IDIMeshObject(meshes)
{
	Initialize(meshes);
}

D3D11MeshObject::D3D11MeshObject(const std::string& basePath, const std::string& fileName)
	: IDIMeshObject(basePath, fileName)
{
	Initialize(basePath, fileName);
}

D3D11MeshObject::~D3D11MeshObject()
{
	for (auto& ptr : mMeshes)
	{
		delete ptr;
	}
}


void D3D11MeshObject::Initialize(const std::string& basePath, const std::string& filename)
{
	auto meshes = GeometryGenerator::ReadFromFile(basePath, filename);
	Initialize(meshes);
}

void D3D11MeshObject::Initialize(const std::vector<MeshData>& meshes)
{
	// CleanUp meshes
	mMeshes.clear();

	// Initialize constant buffer
	{
		// Default
		mMeshConstData.World = Matrix();
		mMeshConstData.WorldInv = mMeshConstData.World.Invert();
		mMeshConstData.WorldInvTranspose = mMeshConstData.WorldInv.Transpose();
		D3D11Utils::CreateConstBuffer(mDevice, mMeshConstData, mMeshConstBuffer);
		D3D11Utils::CreateConstBuffer(mDevice, mMaterialConstData, mMaterialConstBuffer);
	}

	// Initialize mesh datas
	{
		for (const auto& meshData : meshes)
		{
			D3D11Mesh* newMesh = new D3D11Mesh;
			D3D11Utils::CreateVertexBuffer(mDevice, meshData.Vertices, newMesh->VertexBuffer);
			newMesh->mIndexCount = UINT(meshData.Indices.size());
			newMesh->mVertexCount = UINT(meshData.Vertices.size());

			D3D11Utils::CreateIndexBuffer(mDevice, meshData.Indices, newMesh->IndexBuffer);

			if (!meshData.AlbedoTextureFilename.empty())
			{
				D3D11Utils::CreateTexture(mDevice, mContext, meshData.AlbedoTextureFilename, true, newMesh->AlbedoTexture, newMesh->AlbedoSRV);
				mMaterialConstData.useAlbedoMap = 1;
			}
			if (!meshData.EmissiveTextureFilename.empty())
			{
				D3D11Utils::CreateTexture(mDevice, mContext, meshData.EmissiveTextureFilename, true, newMesh->EmissiveTexture, newMesh->EmissiveSRV);
				mMaterialConstData.useEmissiveMap = 1;
			}
			if (!meshData.NormalTextureFilename.empty())
			{
				D3D11Utils::CreateTexture(mDevice, mContext, meshData.NormalTextureFilename, false, newMesh->NormalTexture, newMesh->NormalSRV);
				mMaterialConstData.useNormalMap = 1;
			}
			if (!meshData.HeightTextureFilename.empty())
			{
				D3D11Utils::CreateTexture(mDevice, mContext, meshData.HeightTextureFilename, false, newMesh->HeightTexture, newMesh->HeightSRV);
			}
			if (!meshData.AoTextureFilename.empty())
			{
				D3D11Utils::CreateTexture(mDevice, mContext, meshData.AoTextureFilename, false, newMesh->AoTexture, newMesh->AoSRV);
				mMaterialConstData.useAOMap = 1;
			}
			if (!meshData.MetallicTextureFilename.empty())
			{
				D3D11Utils::CreateTexture(mDevice, mContext, meshData.MetallicTextureFilename, false, newMesh->MetallicTexture, newMesh->MetallicSRV);
				mMaterialConstData.useMetallicMap = 1;
			}
			if (!meshData.RoughnessTextureFilename.empty())
			{
				D3D11Utils::CreateTexture(mDevice, mContext, meshData.RoughnessTextureFilename, false, newMesh->RoughnessTexture, newMesh->RoughnessSRV);
				mMaterialConstData.useRoughnessMap = 1;
			}

			newMesh->MeshConstBuffer = mMeshConstBuffer;
			newMesh->MaterialConstBuffer = mMaterialConstBuffer;

			this->mMeshes.push_back(newMesh);

			// Calc extends
			mExtents.x = std::max(mExtents.x, meshData.Extends.x);
			mExtents.y = std::max(mExtents.y, meshData.Extends.y);
			mExtents.z = std::max(mExtents.z, meshData.Extends.z);
		}
	}
}

void D3D11MeshObject::Render(const Matrix& tm)
{
	// Update Constant buffers
	mMeshConstData.World = tm.Transpose();
	mMeshConstData.WorldInv = mMeshConstData.World.Invert();
	mMeshConstData.WorldInvTranspose = mMeshConstData.WorldInv.Transpose();
	D3D11Utils::UpdateBuffer(mDevice, mContext, mMeshConstData, mMeshConstBuffer);
	D3D11Utils::UpdateBuffer(mDevice, mContext, mMaterialConstData, mMaterialConstBuffer);

	// Render

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	for (const auto& mesh : mMeshes)
	{
		ID3D11Buffer* constBuffers[2] =
		{
			mesh->MeshConstBuffer.Get(),
			mesh->MaterialConstBuffer.Get()
		};
		// Set VS, PS shaders
		mContext->VSSetShaderResources(0, 1, mesh->HeightSRV.GetAddressOf());
		mContext->VSSetConstantBuffers(1, 2, constBuffers);

		// Set textures as SRV
		std::vector<ID3D11ShaderResourceView*> resViews =
		{
			mesh->AlbedoSRV.Get(),
			mesh->NormalSRV.Get(),
			mesh->AoSRV.Get(),
			mesh->MetallicSRV.Get(),
			mesh->RoughnessSRV.Get(),
			mesh->EmissiveSRV.Get()
		};
		mContext->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());
		mContext->PSSetConstantBuffers(1, 2, constBuffers);

		mContext->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		mContext->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Draw object
		mContext->DrawIndexed(mesh->mIndexCount, 0, 0);
	}
}