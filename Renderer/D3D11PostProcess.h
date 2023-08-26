#pragma once
#include "D3D11ImageFilter.h"

class RENDERER_API D3D11PostProcess
{
public:
	void
		Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
			const std::vector<ComPtr<ID3D11ShaderResourceView>>& resources,
			const std::vector<ComPtr<ID3D11RenderTargetView>>& targets,
			const int width, const int height, const int bloomLevels);

	void Render(ComPtr<ID3D11DeviceContext>& context);

	void RenderImageFilter(ComPtr<ID3D11DeviceContext>& context, const D3D11ImageFilter& imageFilter);

	void CreateBuffer(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, int width,
		int height, ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv);

public:
	D3D11ImageFilter mCombineFilter;
	std::vector<D3D11ImageFilter> mBloomDownFilters;
	std::vector<D3D11ImageFilter> mBloomUpFilters;

private:
	std::unique_ptr<D3D11Mesh> mMesh;
	ComPtr<ID3D11InputLayout> mInputLayout;
	ComPtr<ID3D11SamplerState> mSamplerState;
	ComPtr<ID3D11RasterizerState> mRasterizerSate;
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader> mCombinePixelShader;
	ComPtr<ID3D11PixelShader> mBloomDownPixelShader;
	ComPtr<ID3D11PixelShader> mBloomUpPixelShader;

	std::vector<ComPtr<ID3D11ShaderResourceView>> mBloomSRVs;
	std::vector<ComPtr<ID3D11RenderTargetView>> mBloomRTVs;
};