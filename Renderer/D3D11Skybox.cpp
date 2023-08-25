#include "pch.h"
#include "D3D11Skybox.h"

#include "D3D11Utils.h"
#include "GeometryGenerator.h"

using namespace D3D11Common;

static ComPtr<ID3D11Device> mDevice;
static ComPtr<ID3D11DeviceContext> mContext;

void D3D11Skybox::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	mDevice = device;
	mContext = context;
}

void D3D11Skybox::Initialize(
	wstring envFilename,
	wstring specularFilename,
	wstring irradianceFilename,
	wstring brdfFilename)
{
	// Textures
	D3D11Utils::CreateDDSTexture(mDevice, envFilename.c_str(), true, mEnvSRV);
	D3D11Utils::CreateDDSTexture(mDevice, specularFilename.c_str(), true, mSpecularSRV);
	D3D11Utils::CreateDDSTexture(mDevice, irradianceFilename.c_str(), true, mIrradianceSRV);
	D3D11Utils::CreateDDSTexture(mDevice, brdfFilename.c_str(), false, mBrdfSRV);
	// Mesh
	mCubeMesh = std::make_shared<D3D11Mesh>();
	MeshData cubeMeshData = GeometryGenerator::MakeBox(SCALE);
	std::reverse(cubeMeshData.Indices.begin(), cubeMeshData.Indices.end());
	// Buffers
	D3D11Utils::CreateVertexBuffer(mDevice, cubeMeshData.Vertices, mCubeMesh->VertexBuffer); mCubeMesh->mIndexCount = UINT(cubeMeshData.Indices.size());
	D3D11Utils::CreateIndexBuffer(mDevice, cubeMeshData.Indices, mCubeMesh->IndexBuffer);
}

void D3D11Skybox::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	mContext->IASetVertexBuffers(0, 1, mCubeMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	mContext->IASetIndexBuffer(mCubeMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	mContext->DrawIndexed(mCubeMesh->mIndexCount, 0, 0);
}
