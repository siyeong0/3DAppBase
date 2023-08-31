#include "pch.h"

#include "D3D11Utils.h"

#include <DirectXTexEXR.h> // Read .EXR
#include <algorithm>
#include <directxtk/DDSTextureLoader.h> // Read Cubemap
#include <dxgi.h>						// DXGIFactory
#include <dxgi1_4.h>                    // DXGIFactory4
#include <fp16.h>
#include <iostream>

#include "ImageReader.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using namespace DirectX;

void CheckResult(HRESULT hr, ID3DBlob* errorBlob)
{
	if (FAILED(hr))
	{
		if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0)
		{
			cout << "File not found." << endl;
		}

		// 에러 메시지가 있으면 출력
		if (errorBlob)
		{
			cout << "Shader compile error\n" << (char*)errorBlob->GetBufferPointer() << endl;
		}
	}
}

void D3D11Utils::CreateVertexShaderAndInputLayout(
	ComPtr<ID3D11Device>& device,
	const wstring& filename,
	const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
	ComPtr<ID3D11VertexShader>& m_vertexShader,
	ComPtr<ID3D11InputLayout>& m_inputLayout)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
	CheckResult(hr, errorBlob.Get());
	device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &m_vertexShader);
	device->CreateInputLayout(inputElements.data(), UINT(inputElements.size()), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &m_inputLayout);
}

void D3D11Utils::CreateHullShader(ComPtr<ID3D11Device>& device, const wstring& filename, ComPtr<ID3D11HullShader>& m_hullShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "hs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
	CheckResult(hr, errorBlob.Get());
	device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &m_hullShader);
}

void D3D11Utils::CreateDomainShader(ComPtr<ID3D11Device>& device, const wstring& filename, ComPtr<ID3D11DomainShader>& m_domainShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ds_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
	CheckResult(hr, errorBlob.Get());
	device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &m_domainShader);
}

void D3D11Utils::CreatePixelShader(ComPtr<ID3D11Device>& device, const wstring& filename, ComPtr<ID3D11PixelShader>& m_pixelShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
	CheckResult(hr, errorBlob.Get());
	device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &m_pixelShader);
}

void D3D11Utils::CreateIndexBuffer(ComPtr<ID3D11Device>& device, const std::vector<uint32_t>& indices, ComPtr<ID3D11Buffer>& indexBuffer)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;	// IMMUTABLE
	bufferDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;				// 0 if no CPU access is necessary.
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(indexBufferData));
	indexBufferData.pSysMem = indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	device->CreateBuffer(&bufferDesc, &indexBufferData, indexBuffer.GetAddressOf());
}

void D3D11Utils::CreateGeometryShader(ComPtr<ID3D11Device>& device, const wstring& filename, ComPtr<ID3D11GeometryShader>& geometryShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
	// CheckResult(hr, errorBlob.Get());
	device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &geometryShader);
}

void ReadEXRImage(const std::string filename, std::vector<uint8_t>& image, int& width, int& height, DXGI_FORMAT& pixelFormat)
{
	const std::wstring wFilename(filename.begin(), filename.end());

	TexMetadata metadata;
	ThrowIfFailed(GetMetadataFromEXRFile(wFilename.c_str(), metadata));

	ScratchImage scratchImage;
	ThrowIfFailed(LoadFromEXRFile(wFilename.c_str(), NULL, scratchImage));

	width = static_cast<int>(metadata.width);
	height = static_cast<int>(metadata.height);
	pixelFormat = metadata.format;

	cout << filename << " " << metadata.width << " " << metadata.height << metadata.format << endl;

	image.resize(scratchImage.GetPixelsSize());
	memcpy(image.data(), scratchImage.GetPixels(), image.size());

	// Check data range
	vector<float> f32(image.size() / 2);
	uint16_t* f16 = (uint16_t*)image.data();
	for (int i = 0; i < image.size() / 2; i++)
	{
		f32[i] = fp16_ieee_to_fp32_value(f16[i]);
	}
	const float minValue = *std::min_element(f32.begin(), f32.end());
	const float maxValue = *std::max_element(f32.begin(), f32.end());

	cout << minValue << " " << maxValue << endl;
}

ComPtr<ID3D11Texture2D> CreateStagingTexture(
	ComPtr<ID3D11Device>& device,
	ComPtr<ID3D11DeviceContext>& context,
	const int width, const int height,
	const std::vector<uint8_t>& image,
	const DXGI_FORMAT pixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
	const int mipLevels = 1, const int arraySize = 1)
{
	// Create Staging Texture
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = mipLevels;
	txtDesc.ArraySize = arraySize;
	txtDesc.Format = pixelFormat;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_STAGING;
	txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

	ComPtr<ID3D11Texture2D> stagingTexture;
	if (FAILED(device->CreateTexture2D(&txtDesc, NULL, stagingTexture.GetAddressOf())))
	{
		cout << "Failed()" << endl;
	}

	// Copy image from cpu
	size_t pixelSize = sizeof(uint8_t) * 4;
	if (pixelFormat == DXGI_FORMAT_R16G16B16A16_FLOAT)
	{
		pixelSize = sizeof(uint16_t) * 4;
	}
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
	uint8_t* pData = (uint8_t*)ms.pData;
	for (UINT h = 0; h < UINT(height); h++)
	{
		memcpy(&pData[h * ms.RowPitch], &image[h * width * pixelSize], width * pixelSize);
	}
	context->Unmap(stagingTexture.Get(), NULL);

	return stagingTexture;
}

void D3D11Utils::CreateTexture(
	ComPtr<ID3D11Device>& device,
	ComPtr<ID3D11DeviceContext>& context,
	const std::string filename, const bool usSRGB,
	ComPtr<ID3D11Texture2D>& texture,
	ComPtr<ID3D11ShaderResourceView>& textureResourceView)
{
	int width = 0, height = 0;
	std::vector<uint8_t> image;
	DXGI_FORMAT pixelFormat = usSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

	string ext(filename.end() - 3, filename.end());
	std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower);

	if (ext == "exr")
	{
		ReadEXRImage(filename, image, width, height, pixelFormat);
	}
	else
	{
		ReadImage(filename, image, width, height);
	}

	// Create Stagin Texture
	ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(device, context, width, height, image, pixelFormat);

	// Target Texture
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = 0;
	txtDesc.ArraySize = 1;
	txtDesc.Format = pixelFormat;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_DEFAULT;
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	txtDesc.CPUAccessFlags = 0;

	// Clear to black
	device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());

	// Copy from staging texture in max resolution
	context->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0, stagingTexture.Get(), 0, NULL);

	// ResourceView
	device->CreateShaderResourceView(texture.Get(), 0,
		textureResourceView.GetAddressOf());

	// Generate mipmaps
	context->GenerateMips(textureResourceView.Get());
}

void D3D11Utils::CreateTextureArray(
	ComPtr<ID3D11Device>& device,
	ComPtr<ID3D11DeviceContext>& context,
	const std::vector<std::string> filenames,
	ComPtr<ID3D11Texture2D>& texture,
	ComPtr<ID3D11ShaderResourceView>& textureResourceView)
{
	using namespace std;

	if (filenames.empty())
	{
		return;
	}

	// Assert width == height
	int width = 0, height = 0;
	vector<vector<uint8_t>> imageArray;
	for (const auto& f : filenames)
	{
		cout << f << endl;
		std::vector<uint8_t> image;
		ReadImage(f, image, width, height);
		imageArray.push_back(image);
	}

	UINT size = UINT(filenames.size());

	// Create Texture2DArray
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = UINT(width);
	txtDesc.Height = UINT(height);
	txtDesc.MipLevels = 0;
	txtDesc.ArraySize = size;
	txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.SampleDesc.Quality = 0;
	txtDesc.Usage = D3D11_USAGE_DEFAULT;
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Clear to black
	device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());
	// Check mipmap
	texture->GetDesc(&txtDesc);

	// Copy
	for (size_t i = 0; i < imageArray.size(); i++)
	{
		auto& image = imageArray[i];
		ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(device, context, width, height, image, txtDesc.Format, 1, 1);

		UINT subresourceIndex = D3D11CalcSubresource(0, UINT(i), txtDesc.MipLevels);

		context->CopySubresourceRegion(texture.Get(), subresourceIndex, 0, 0, 0, stagingTexture.Get(), 0, NULL);
	}

	device->CreateShaderResourceView(texture.Get(), NULL, textureResourceView.GetAddressOf());

	context->GenerateMips(textureResourceView.Get());
}

void D3D11Utils::CreateDDSTexture(
	ComPtr<ID3D11Device>& device,
	const wchar_t* filename, bool isCubeMap,
	ComPtr<ID3D11ShaderResourceView>& textureResourceView)
{
	ComPtr<ID3D11Texture2D> texture;
	UINT miscFlags = 0;
	if (isCubeMap)
	{
		miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	// https://github.com/microsoft/DirectXTK/wiki/DDSTextureLoader
	ThrowIfFailed(CreateDDSTextureFromFileEx(
		device.Get(), filename, 0, D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE, 0, miscFlags, DDS_LOADER_FLAGS(false),
		(ID3D11Resource**)texture.GetAddressOf(),
		textureResourceView.GetAddressOf(), NULL));
}

void D3D11Utils::WriteToFile(
	ComPtr<ID3D11Device>& device,
	ComPtr<ID3D11DeviceContext>& context,
	ComPtr<ID3D11Texture2D>& textureToWrite,
	const std::string filename)
{
	D3D11_TEXTURE2D_DESC desc;
	textureToWrite->GetDesc(&desc);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;

	ComPtr<ID3D11Texture2D> stagingTexture;
	if (FAILED(device->CreateTexture2D(&desc, NULL, stagingTexture.GetAddressOf())))
	{
		cout << "Failed()" << endl;
	}

	// Copy all
	// context->CopyResource(stagingTexture.Get(), pTemp.Get());

	// Copy part
	D3D11_BOX box;
	box.left = 0;
	box.right = desc.Width;
	box.top = 0;
	box.bottom = desc.Height;
	box.front = 0;
	box.back = 1;
	context->CopySubresourceRegion(stagingTexture.Get(), 0, 0, 0, 0, textureToWrite.Get(), 0, &box);

	// Assert R8G8B8A8
	std::vector<uint8_t> pixels(desc.Width * desc.Height * 4);

	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(stagingTexture.Get(), NULL, D3D11_MAP_READ, NULL, &ms);

	uint8_t* pData = (uint8_t*)ms.pData;
	for (unsigned int h = 0; h < desc.Height; h++)
	{
		memcpy(&pixels[h * desc.Width * 4], &pData[h * ms.RowPitch], desc.Width * sizeof(uint8_t) * 4);
	}

	context->Unmap(stagingTexture.Get(), NULL);

	stbi_write_png(filename.c_str(), desc.Width, desc.Height, 4, pixels.data(), desc.Width * 4);

	cout << filename << endl;
}
