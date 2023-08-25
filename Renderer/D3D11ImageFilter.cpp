#include "pch.h"
#include "D3D11ImageFilter.h"

ImageFilter::ImageFilter(
	ComPtr<ID3D11Device>& device,
	ComPtr<ID3D11DeviceContext>& context,
	ComPtr<ID3D11PixelShader>& pixelShader,
	int width, int height)
{
	Initialize(device, context, pixelShader, width, height);
}

void ImageFilter::Initialize(
	ComPtr<ID3D11Device>& device,
	ComPtr<ID3D11DeviceContext>& context,
	ComPtr<ID3D11PixelShader>& pixelShader,
	int width, int height)
{
	pixelShader.CopyTo(mPixelShader.GetAddressOf());

	ZeroMemory(&mViewport, sizeof(D3D11_VIEWPORT));
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = float(width);
	mViewport.Height = float(height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mConstData.DeltaX = 1.0f / width;
	mConstData.DeltaY = 1.0f / height;

	D3D11Utils::CreateConstBuffer(device, mConstData, mConstBuffer);
}

void ImageFilter::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	D3D11Utils::UpdateBuffer(device, context, mConstData, mConstBuffer);
}

void ImageFilter::Render(ComPtr<ID3D11DeviceContext>& context) const
{
	assert(mShaderResources.size() > 0);
	assert(mRenderTargets.size() > 0);

	context->RSSetViewports(1, &mViewport);
	context->OMSetRenderTargets(UINT(mRenderTargets.size()), mRenderTargets.data(), NULL);
	context->PSSetShader(mPixelShader.Get(), 0, 0);
	context->PSSetShaderResources(0, UINT(mShaderResources.size()), mShaderResources.data());
	context->PSSetConstantBuffers(0, 1, mConstBuffer.GetAddressOf());
}

void ImageFilter::SetShaderResources(const std::vector<ComPtr<ID3D11ShaderResourceView>>& resources)
{
	mShaderResources.clear();
	for (const auto& res : resources)
	{
		mShaderResources.push_back(res.Get());
	}
}

void ImageFilter::SetRenderTargets(const std::vector<ComPtr<ID3D11RenderTargetView>>& targets)
{
	mRenderTargets.clear();
	for (const auto& tar : targets)
	{
		mRenderTargets.push_back(tar.Get());
	}
}