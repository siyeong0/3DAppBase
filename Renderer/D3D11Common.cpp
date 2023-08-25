#include "pch.h"
#include "D3D11Common.h"

namespace D3D11Common
{
	// Sampler States
	ComPtr<ID3D11SamplerState> LinearWrapSS;
	ComPtr<ID3D11SamplerState> LinearClampSS;
	ComPtr<ID3D11SamplerState> PointClampSS;
	ComPtr<ID3D11SamplerState> ShadowPointSS;
	ComPtr<ID3D11SamplerState> ShadowCompareSS;
	ComPtr<ID3D11SamplerState> PointWrapSS;
	ComPtr<ID3D11SamplerState> LinearMirrorSS;
	std::vector<ID3D11SamplerState*> sampleStates;

	// Rasterizer States
	ComPtr<ID3D11RasterizerState> SolidRS; // front only
	ComPtr<ID3D11RasterizerState> SolidCcwRS;
	ComPtr<ID3D11RasterizerState> WireRS;
	ComPtr<ID3D11RasterizerState> WireCcwRS;
	ComPtr<ID3D11RasterizerState> PostProcessingRS;
	ComPtr<ID3D11RasterizerState> SolidBothRS; // front and back
	ComPtr<ID3D11RasterizerState> WireBothRS;
	ComPtr<ID3D11RasterizerState> SolidBothCcwRS;
	ComPtr<ID3D11RasterizerState> WireBothCcwRS;

	// Depth Stencil States
	ComPtr<ID3D11DepthStencilState> DrawDSS;       // 일반적으로 그리기
	ComPtr<ID3D11DepthStencilState> MaskDSS;       // 스텐실버퍼에 표시
	ComPtr<ID3D11DepthStencilState> DrawMaskedDSS; // 스텐실 표시된 곳만

	// Blend States
	ComPtr<ID3D11BlendState> MirrorBS;
	ComPtr<ID3D11BlendState> AccumulateBS;
	ComPtr<ID3D11BlendState> AlphaBS;

	// Shaders
	ComPtr<ID3D11VertexShader> BasicVS;
	ComPtr<ID3D11VertexShader> SkyboxVS;
	ComPtr<ID3D11VertexShader> SamplingVS;
	ComPtr<ID3D11VertexShader> NormalVS;
	ComPtr<ID3D11VertexShader> DepthOnlyVS;

	ComPtr<ID3D11PixelShader> BasicPS;
	ComPtr<ID3D11PixelShader> SkyboxPS;
	ComPtr<ID3D11PixelShader> CombinePS;
	ComPtr<ID3D11PixelShader> BloomDownPS;
	ComPtr<ID3D11PixelShader> BloomUpPS;
	ComPtr<ID3D11PixelShader> NormalPS;
	ComPtr<ID3D11PixelShader> DepthOnlyPS;
	ComPtr<ID3D11PixelShader> ColorPS;

	ComPtr<ID3D11GeometryShader> NormalGS;

	// Input Layouts
	ComPtr<ID3D11InputLayout> BasicIL;
	ComPtr<ID3D11InputLayout> SamplingIL;
	ComPtr<ID3D11InputLayout> SkyboxIL;

	// Graphics Pipeline States
	GraphicsPSO DefaultSolidPSO;
	GraphicsPSO DefaultWirePSO;
	GraphicsPSO StencilMaskPSO;
	GraphicsPSO ReflectSolidPSO;
	GraphicsPSO ReflectWirePSO;
	GraphicsPSO MirrorBlendSolidPSO;
	GraphicsPSO MirrorBlendWirePSO;
	GraphicsPSO SkyboxSolidPSO;
	GraphicsPSO SkyboxWirePSO;
	GraphicsPSO ReflectSkyboxSolidPSO;
	GraphicsPSO ReflectSkyboxWirePSO;
	GraphicsPSO NormalsPSO;
	GraphicsPSO DepthOnlyPSO;
	GraphicsPSO PostProcessingPSO;

} // namespace D3D11Common

void D3D11Common::InitCommonStates(ComPtr<ID3D11Device>& device)
{
	InitShaders(device);
	InitSamplers(device);
	InitRasterizerStates(device);
	InitBlendStates(device);
	InitDepthStencilStates(device);
	InitPipelineStates(device);
}

void D3D11Common::SetPipelineState(ComPtr<ID3D11DeviceContext>& context, const GraphicsPSO& pso)
{
	context->VSSetShader(pso.VertexShader.Get(), 0, 0);
	context->PSSetShader(pso.PixelShader.Get(), 0, 0);
	context->HSSetShader(pso.HullShader.Get(), 0, 0);
	context->DSSetShader(pso.DomainShader.Get(), 0, 0);
	context->GSSetShader(pso.GeometryShader.Get(), 0, 0);
	context->CSSetShader(NULL, 0, 0);
	context->IASetInputLayout(pso.InputLayout.Get());
	context->RSSetState(pso.RasterizerState.Get());
	context->OMSetBlendState(pso.BlendState.Get(), pso.BlendFactor, 0xffffffff);
	context->OMSetDepthStencilState(pso.DepthStencilState.Get(), pso.StencilRef);
	context->IASetPrimitiveTopology(pso.PrimitiveTopology);
}

void D3D11Common::InitSamplers(ComPtr<ID3D11Device>& device)
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, LinearWrapSS.GetAddressOf());

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	device->CreateSamplerState(&sampDesc, PointWrapSS.GetAddressOf());

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(&sampDesc, LinearClampSS.GetAddressOf());

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	device->CreateSamplerState(&sampDesc, PointClampSS.GetAddressOf());

	// shadowPointSS
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = 1.0f; // 큰 Z값
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	device->CreateSamplerState(&sampDesc, ShadowPointSS.GetAddressOf());

	// shadowCompareSS, 쉐이더 안에서는 SamplerComparisonState
	// Filter = "_COMPARISON_" 주의
	// https://www.gamedev.net/forums/topic/670575-uploading-samplercomparisonstate-in-hlsl/
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = 100.0f; // 큰 Z값
	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateSamplerState(&sampDesc, ShadowCompareSS.GetAddressOf());

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, LinearMirrorSS.GetAddressOf());

	// 샘플러 순서가 "Common.hlsli"에서와 일관성 있어야 함
	sampleStates.push_back(LinearWrapSS.Get());    // s0
	sampleStates.push_back(LinearClampSS.Get());   // s1
	sampleStates.push_back(ShadowPointSS.Get());   // s2
	sampleStates.push_back(ShadowCompareSS.Get()); // s3
	sampleStates.push_back(PointWrapSS.Get());     // s4
	sampleStates.push_back(LinearMirrorSS.Get());  // s5
	sampleStates.push_back(PointClampSS.Get());    // s6
}

void D3D11Common::InitRasterizerStates(ComPtr<ID3D11Device>& device)
{
	// Rasterizer States
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, SolidRS.GetAddressOf()));

	// 거울에 반사되면 삼각형의 Winding이 바뀌기 때문에 CCW로 그려야함
	rasterDesc.FrontCounterClockwise = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, SolidCcwRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, WireCcwRS.GetAddressOf()));

	rasterDesc.FrontCounterClockwise = false;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, WireRS.GetAddressOf()));

	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE; // 양면
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, SolidBothRS.GetAddressOf()));

	rasterDesc.FrontCounterClockwise = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, SolidBothCcwRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME; // 양면, Wire
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, WireBothCcwRS.GetAddressOf()));

	rasterDesc.FrontCounterClockwise = false;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, WireBothRS.GetAddressOf()));

	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthClipEnable = false;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, PostProcessingRS.GetAddressOf()));
}

void D3D11Common::InitBlendStates(ComPtr<ID3D11Device>& device)
{
	// "이미 그려져있는 화면"과 어떻게 섞을지를 결정
	// Dest: 이미 그려져 있는 값들을 의미
	// Src: 픽셀 쉐이더가 계산한 값들을 의미 (여기서는 마지막 거울)

	D3D11_BLEND_DESC mirrorBlendDesc;
	ZeroMemory(&mirrorBlendDesc, sizeof(mirrorBlendDesc));
	mirrorBlendDesc.AlphaToCoverageEnable = true; // MSAA
	mirrorBlendDesc.IndependentBlendEnable = false;
	// 개별 RenderTarget에 대해서 설정 (최대 8개)
	mirrorBlendDesc.RenderTarget[0].BlendEnable = true;
	mirrorBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	mirrorBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
	mirrorBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	mirrorBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	mirrorBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	mirrorBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// 필요하면 RGBA 각각에 대해서도 조절 가능
	mirrorBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ThrowIfFailed(device->CreateBlendState(&mirrorBlendDesc, MirrorBS.GetAddressOf()));

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = true; // MSAA
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR; // INV 아님
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	ThrowIfFailed(device->CreateBlendState(&blendDesc, AccumulateBS.GetAddressOf()));

	// Dst: 현재 백버퍼, Src: 새로 픽셀 쉐이더에서 출력,
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false; // <- 주의: FALSE
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	ThrowIfFailed(device->CreateBlendState(&blendDesc, AlphaBS.GetAddressOf()));
}

void D3D11Common::InitDepthStencilStates(ComPtr<ID3D11Device>& device)
{
	// D3D11_DEPTH_STENCIL_DESC 옵션 정리
	// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_depth_stencil_desc
	// StencilRead/WriteMask: 예) uint8 중 어떤 비트를 사용할지

	// D3D11_DEPTH_STENCILOP_DESC 옵션 정리
	// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_depth_stencilop_desc
	// StencilPassOp : 둘 다 pass일 때 할 일
	// StencilDepthFailOp : Stencil pass, Depth fail 일 때 할 일
	// StencilFailOp : 둘 다 fail 일 때 할 일

	// m_drawDSS: 기본 DSS
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false; // Stencil 불필요
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	// 앞면에 대해서 어떻게 작동할지 설정
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// 뒷면에 대해 어떻게 작동할지 설정 (뒷면도 그릴 경우)
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, DrawDSS.GetAddressOf()));

	// Stencil에 1로 표기해주는 DSS
	dsDesc.DepthEnable = true; // 이미 그려진 물체 유지
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = true;    // Stencil 필수
	dsDesc.StencilReadMask = 0xFF;  // 모든 비트 다 사용
	dsDesc.StencilWriteMask = 0xFF; // 모든 비트 다 사용
	// 앞면에 대해서 어떻게 작동할지 설정
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, MaskDSS.GetAddressOf()));

	// Stencil에 1로 표기된 경우에"만" 그리는 DSS
	// DepthBuffer는 초기화된 상태로 가정
	// D3D11_COMPARISON_EQUAL 이미 1로 표기된 경우에만 그리기
	// OMSetDepthStencilState(..., 1); <- 여기의 1
	dsDesc.DepthEnable = true;   // 거울 속을 다시 그릴때 필요
	dsDesc.StencilEnable = true; // Stencil 사용
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // <- 주의
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, DrawMaskedDSS.GetAddressOf()));
}

void D3D11Common::InitShaders(ComPtr<ID3D11Device>& device)
{

	// Shaders, InputLayouts
	std::vector<D3D11_INPUT_ELEMENT_DESC> basicIEs =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> samplingIED =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> skyboxIE =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"../Renderer/Shaders//BasicVS.hlsl", basicIEs, BasicVS, BasicIL);
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"../Renderer/Shaders/NormalVS.hlsl", basicIEs, NormalVS, BasicIL);
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"../Renderer/Shaders/SamplingVS.hlsl", samplingIED, SamplingVS, SamplingIL);
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"../Renderer/Shaders/SkyboxVS.hlsl", skyboxIE, SkyboxVS, SkyboxIL);
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"../Renderer/Shaders/DepthOnlyVS.hlsl", basicIEs, DepthOnlyVS, SkyboxIL);

	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/BasicPS.hlsl", BasicPS);
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/NormalPS.hlsl", NormalPS);
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/SkyboxPS.hlsl", SkyboxPS);
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/CombinePS.hlsl", CombinePS);
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/BloomDownPS.hlsl", BloomDownPS);
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/BloomUpPS.hlsl", BloomUpPS);
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/DepthOnlyPS.hlsl", DepthOnlyPS);
	D3D11Utils::CreatePixelShader(device, L"../Renderer/Shaders/ColorPS.hlsl", ColorPS);

	D3D11Utils::CreateGeometryShader(device, L"../Renderer/Shaders/NormalGS.hlsl", NormalGS);
}

void D3D11Common::InitPipelineStates(ComPtr<ID3D11Device>& device)
{
	// defaultSolidPSO;
	DefaultSolidPSO.VertexShader = BasicVS;
	DefaultSolidPSO.InputLayout = BasicIL;
	DefaultSolidPSO.PixelShader = BasicPS;
	DefaultSolidPSO.RasterizerState = SolidRS;
	DefaultSolidPSO.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// defaultWirePSO
	DefaultWirePSO = DefaultSolidPSO;
	DefaultWirePSO.RasterizerState = WireRS;

	// stencilMarkPSO;
	StencilMaskPSO = DefaultSolidPSO;
	StencilMaskPSO.DepthStencilState = MaskDSS;
	StencilMaskPSO.StencilRef = 1;
	StencilMaskPSO.VertexShader = DepthOnlyVS;
	StencilMaskPSO.PixelShader = DepthOnlyPS;

	// reflectSolidPSO: 반사되면 Winding 반대
	ReflectSolidPSO = DefaultSolidPSO;
	ReflectSolidPSO.DepthStencilState = DrawMaskedDSS;
	ReflectSolidPSO.RasterizerState = SolidCcwRS; // 반시계
	ReflectSolidPSO.StencilRef = 1;

	// reflectWirePSO: 반사되면 Winding 반대
	ReflectWirePSO = ReflectSolidPSO;
	ReflectWirePSO.RasterizerState = WireCcwRS; // 반시계
	ReflectWirePSO.StencilRef = 1;

	// mirrorBlendSolidPSO;
	MirrorBlendSolidPSO = DefaultSolidPSO;
	MirrorBlendSolidPSO.BlendState = MirrorBS;
	MirrorBlendSolidPSO.DepthStencilState = DrawMaskedDSS;
	MirrorBlendSolidPSO.StencilRef = 1;

	// mirrorBlendWirePSO;
	MirrorBlendWirePSO = DefaultWirePSO;
	MirrorBlendWirePSO.BlendState = MirrorBS;
	MirrorBlendWirePSO.DepthStencilState = DrawMaskedDSS;
	MirrorBlendWirePSO.StencilRef = 1;

	// skyboxSolidPSO
	SkyboxSolidPSO = DefaultSolidPSO;
	SkyboxSolidPSO.VertexShader = SkyboxVS;
	SkyboxSolidPSO.PixelShader = SkyboxPS;
	SkyboxSolidPSO.InputLayout = SkyboxIL;

	// skyboxWirePSO
	SkyboxWirePSO = SkyboxSolidPSO;
	SkyboxWirePSO.RasterizerState = WireRS;

	// reflectSkyboxSolidPSO
	ReflectSkyboxSolidPSO = SkyboxSolidPSO;
	ReflectSkyboxSolidPSO.DepthStencilState = DrawMaskedDSS;
	ReflectSkyboxSolidPSO.RasterizerState = SolidCcwRS; // 반시계
	ReflectSkyboxSolidPSO.StencilRef = 1;

	// reflectSkyboxWirePSO
	ReflectSkyboxWirePSO = ReflectSkyboxSolidPSO;
	ReflectSkyboxWirePSO.RasterizerState = WireCcwRS;
	ReflectSkyboxWirePSO.StencilRef = 1;

	// normalsPSO
	NormalsPSO = DefaultSolidPSO;
	NormalsPSO.VertexShader = NormalVS;
	NormalsPSO.GeometryShader = NormalGS;
	NormalsPSO.PixelShader = NormalPS;
	NormalsPSO.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	// depthOnlyPSO
	DepthOnlyPSO = DefaultSolidPSO;
	DepthOnlyPSO.VertexShader = DepthOnlyVS;
	DepthOnlyPSO.PixelShader = DepthOnlyPS;

	// postProcessingPSO
	PostProcessingPSO.VertexShader = SamplingVS;
	PostProcessingPSO.PixelShader = DepthOnlyPS; // dummy
	PostProcessingPSO.InputLayout = SamplingIL;
	PostProcessingPSO.RasterizerState = PostProcessingRS;
}