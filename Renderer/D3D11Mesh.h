#pragma once
#include "dllAPI.h"
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct RENDERER_API D3D11Mesh
{
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;

	ComPtr<ID3D11Buffer> MeshConstBuffer;
	ComPtr<ID3D11Buffer> MaterialConstBuffer;

	ComPtr<ID3D11Texture2D> AlbedoTexture;
	ComPtr<ID3D11Texture2D> EmissiveTexture;
	ComPtr<ID3D11Texture2D> NormalTexture;
	ComPtr<ID3D11Texture2D> HeightTexture;
	ComPtr<ID3D11Texture2D> AoTexture;
	ComPtr<ID3D11Texture2D> MetallicTexture;
	ComPtr<ID3D11Texture2D> RoughnessTexture;

	ComPtr<ID3D11ShaderResourceView> AlbedoSRV;
	ComPtr<ID3D11ShaderResourceView> EmissiveSRV;
	ComPtr<ID3D11ShaderResourceView> NormalSRV;
	ComPtr<ID3D11ShaderResourceView> HeightSRV;
	ComPtr<ID3D11ShaderResourceView> AoSRV;
	ComPtr<ID3D11ShaderResourceView> MetallicSRV;
	ComPtr<ID3D11ShaderResourceView> RoughnessSRV;

	UINT mIndexCount = 0;
	UINT mVertexCount = 0;
};