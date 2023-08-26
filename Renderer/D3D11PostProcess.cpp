#include "pch.h"
#include "D3D11PostProcess.h"

void D3D11PostProcess::Initialize(
	ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
	const std::vector<ComPtr<ID3D11ShaderResourceView>>& resources,
	const std::vector<ComPtr<ID3D11RenderTargetView>>& targets, const int width,
	const int height, const int bloomLevels)
{
	MeshData meshData = GeometryGenerator::MakeSquare();

	mMesh = std::make_unique<D3D11Mesh>();
	D3D11Utils::CreateVertexBuffer(device, meshData.Vertices, mMesh->VertexBuffer);
	mMesh->mIndexCount = UINT(meshData.Indices.size());
	D3D11Utils::CreateIndexBuffer(device, meshData.Indices, mMesh->IndexBuffer);

	std::vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// Common shaders of PostProcess
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"../Renderer/Shaders/SamplingVS.hlsl", basicInputElements, mVertexShader, mInputLayout);
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/CombinePS.hlsl", mCombinePixelShader);
	// Bloom shaders
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/BloomDownPS.hlsl", mBloomDownPixelShader);
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/BloomUpPS.hlsl", mBloomUpPixelShader);

	// Common sampler
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());

	// Rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthClipEnable = false;
	device->CreateRasterizerState(&rastDesc, mRasterizerSate.GetAddressOf());

	// Bloom Down/Up
	mBloomSRVs.resize(bloomLevels);
	mBloomRTVs.resize(bloomLevels);
	for (int i = 0; i < bloomLevels; i++)
	{
		int div = int(pow(2, i));
		CreateBuffer(device, context, width / div, height / div, mBloomSRVs[i], mBloomRTVs[i]);
	}

	mBloomDownFilters.resize(bloomLevels - 1);
	for (int i = 0; i < bloomLevels - 1; i++)
	{
		int div = int(pow(2, i + 1));
		mBloomDownFilters[i].Initialize(device, context, mBloomDownPixelShader, width / div, height / div);
		if (i == 0)
		{
			mBloomDownFilters[i].SetShaderResources({ resources[0] });
		}
		else
		{
			mBloomDownFilters[i].SetShaderResources({ mBloomSRVs[i] });
		}
		mBloomDownFilters[i].SetRenderTargets({ mBloomRTVs[i + 1] });
	}

	mBloomUpFilters.resize(bloomLevels - 1);
	for (int i = 0; i < bloomLevels - 1; i++)
	{
		int level = bloomLevels - 2 - i;
		int div = int(pow(2, level));
		mBloomUpFilters[i].Initialize(device, context, mBloomUpPixelShader, width / div, height / div);
		mBloomUpFilters[i].SetShaderResources({ mBloomSRVs[level + 1] });
		mBloomUpFilters[i].SetRenderTargets({ mBloomRTVs[level] });
	}

	// Combine + ToneMapping
	mCombineFilter.Initialize(device, context, mCombinePixelShader, width, height);
	mCombineFilter.SetShaderResources({ resources[0], mBloomSRVs[0] });
	mCombineFilter.SetRenderTargets(targets);
	mCombineFilter.mConstData.Strength = 0.0f; // Bloom strength
	mCombineFilter.mConstData.Option1 = 1.0f;  // Exposure
	mCombineFilter.mConstData.Option2 = 2.2f;  // Gamma
	mCombineFilter.UpdateConstantBuffers(device, context);
}

void D3D11PostProcess::Render(ComPtr<ID3D11DeviceContext>& context)
{
	context->RSSetState(mRasterizerSate.Get());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetInputLayout(mInputLayout.Get());
	context->IASetVertexBuffers(0, 1, mMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // TODO
	context->VSSetShader(mVertexShader.Get(), 0, 0);
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

	for (int i = 0; i < mBloomDownFilters.size(); i++)
	{
		RenderImageFilter(context, mBloomDownFilters[i]);
	}

	for (int i = 0; i < mBloomUpFilters.size(); i++)
	{
		RenderImageFilter(context, mBloomUpFilters[i]);
	}

	RenderImageFilter(context, mCombineFilter);
}

void D3D11PostProcess::RenderImageFilter(ComPtr<ID3D11DeviceContext>& context, const D3D11ImageFilter& imageFilter)
{
	imageFilter.Render(context);
	context->DrawIndexed(mMesh->mIndexCount, 0, 0);
}

void D3D11PostProcess::CreateBuffer(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
	int width, int height, ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv)
{
	ComPtr<ID3D11Texture2D> texture;

	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = txtDesc.ArraySize = 1;
	txtDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; //  이미지 처리용도
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_DEFAULT;
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	txtDesc.MiscFlags = 0;
	txtDesc.CPUAccessFlags = 0;

	ThrowIfFailed(device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(texture.Get(), NULL, rtv.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf()));
}