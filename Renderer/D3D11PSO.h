#pragma once
#include "dllAPI.h"
#include "D3D11Utils.h"

class RENDERER_API GraphicsPSO
{
public:
	void operator=(const GraphicsPSO& pso);
	void SetBlendFactor(const float blendFactor[4]);

public:
	ComPtr<ID3D11VertexShader> VertexShader;
	ComPtr<ID3D11PixelShader> PixelShader;
	ComPtr<ID3D11HullShader> HullShader;
	ComPtr<ID3D11DomainShader> DomainShader;
	ComPtr<ID3D11GeometryShader> GeometryShader;
	ComPtr<ID3D11InputLayout> InputLayout;

	ComPtr<ID3D11BlendState> BlendState;
	ComPtr<ID3D11DepthStencilState> DepthStencilState;
	ComPtr<ID3D11RasterizerState> RasterizerState;

	float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	UINT StencilRef = 0;

	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};