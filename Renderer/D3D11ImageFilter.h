#pragma once

#include "D3D11Utils.h"
#include "GeometryGenerator.h"
#include "D3D11Mesh.h"

using DirectX::SimpleMath::Vector4;

class RENDERER_API D3D11ImageFilter
{
public:
	D3D11ImageFilter() {};

	D3D11ImageFilter(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11PixelShader>& pixelShader,
		int width, int height);
	void Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11PixelShader>& pixelShader,
		int width, int height);

	void UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);
	void Render(ComPtr<ID3D11DeviceContext>& context) const;
	void SetShaderResources(const std::vector<ComPtr<ID3D11ShaderResourceView>>& resources);
	void SetRenderTargets(const std::vector<ComPtr<ID3D11RenderTargetView>>& targets);

public:
	struct ImageFilterConstData
	{
		float DeltaX;
		float DeltaY;
		float Threshold;
		float Strength;
		float Option1;
		float Option2;
		float Option3;
		float Option4;
	};

	ImageFilterConstData mConstData = {};

protected:
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11Buffer> mConstBuffer;
	D3D11_VIEWPORT mViewport = {};

	// Do not delete pointers
	std::vector<ID3D11ShaderResourceView*> mShaderResources;
	std::vector<ID3D11RenderTargetView*> mRenderTargets;
};