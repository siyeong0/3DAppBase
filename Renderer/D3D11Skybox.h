#pragma once
#include <string>
#include <wrl/client.h>

#include "D3D11Mesh.h"
#include "D3D11Common.h"

using Microsoft::WRL::ComPtr;
using std::wstring;

class RENDERER_API D3D11Skybox
{
public:
	static constexpr float SCALE = 100.0f;
public:
	static void Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);
public:
	void Initialize(
		wstring envFilename,
		wstring specularFilename,
		wstring irradianceFilename,
		wstring brdfFilename);

	void Render();

public:
	struct PixelConstData
	{
		int TextureToDraw = 0;
		float MipLevel = 0.0f;
		float Dummy1 = {};
		float Dummy2 = {};
	};

public:
	bool bWire = false;
	ComPtr<ID3D11ShaderResourceView> mEnvSRV;
	ComPtr<ID3D11ShaderResourceView> mSpecularSRV;   // Radiance
	ComPtr<ID3D11ShaderResourceView> mIrradianceSRV; // Diffuse
	ComPtr<ID3D11ShaderResourceView> mBrdfSRV;       // BRDF LookUpTable

private:
	std::shared_ptr<D3D11Mesh> mCubeMesh;
};