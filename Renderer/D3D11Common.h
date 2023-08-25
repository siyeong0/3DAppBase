#pragma once

#include "D3D11Utils.h"
#include "D3D11PSO.h"


namespace D3D11Common
{
	// Samplers
	extern ComPtr<ID3D11SamplerState> LinearWrapSS;
	extern ComPtr<ID3D11SamplerState> LinearClampSS;
	extern ComPtr<ID3D11SamplerState> PointClampSS;
	extern ComPtr<ID3D11SamplerState> ShadowPointSS;
	extern ComPtr<ID3D11SamplerState> ShadowCompareSS;
	extern ComPtr<ID3D11SamplerState> PointWrapSS;
	extern ComPtr<ID3D11SamplerState> LinearMirrorSS;
	extern std::vector<ID3D11SamplerState*> sampleStates;

	// Rasterizer States
	// CCW: Counter-Clockwise (반시계 방향을 의미합니다.)
	extern ComPtr<ID3D11RasterizerState> SolidRS; // front only
	extern ComPtr<ID3D11RasterizerState> SolidCcwRS;
	extern ComPtr<ID3D11RasterizerState> WireRS;
	extern ComPtr<ID3D11RasterizerState> WireCcwRS;
	extern ComPtr<ID3D11RasterizerState> PostProcessingRS;
	extern ComPtr<ID3D11RasterizerState> SolidBothRS; // front and back
	extern ComPtr<ID3D11RasterizerState> WireBothRS;
	extern ComPtr<ID3D11RasterizerState> SolidBothCcwRS;
	extern ComPtr<ID3D11RasterizerState> WireBothCcwRS;

	// Depth Stencil States
	extern ComPtr<ID3D11DepthStencilState> DrawDSS; // 일반적으로 그리기
	extern ComPtr<ID3D11DepthStencilState> MaskDSS; // 스텐실버퍼에 표시
	extern ComPtr<ID3D11DepthStencilState> DrawMaskedDSS; // 스텐실 표시된 곳만

	// Shaders
	extern ComPtr<ID3D11VertexShader> BasicVS;
	extern ComPtr<ID3D11VertexShader> SkyboxVS;
	extern ComPtr<ID3D11VertexShader> SamplingVS;
	extern ComPtr<ID3D11VertexShader> NormalVS;
	extern ComPtr<ID3D11VertexShader> DepthOnlyVS;

	extern ComPtr<ID3D11PixelShader> BasicPS;
	extern ComPtr<ID3D11PixelShader> SkyboxPS;
	extern ComPtr<ID3D11PixelShader> CombinePS;
	extern ComPtr<ID3D11PixelShader> BloomDownPS;
	extern ComPtr<ID3D11PixelShader> BloomUpPS;
	extern ComPtr<ID3D11PixelShader> NormalPS;
	extern ComPtr<ID3D11PixelShader> DepthOnlyPS;
	extern ComPtr<ID3D11PixelShader> ColorPS;

	extern ComPtr<ID3D11GeometryShader> NormalGS;

	// Input Layouts
	extern ComPtr<ID3D11InputLayout> BasicIL;
	extern ComPtr<ID3D11InputLayout> SamplingIL;
	extern ComPtr<ID3D11InputLayout> SkyboxIL;

	// Blend States
	extern ComPtr<ID3D11BlendState> MirrorBS;
	extern ComPtr<ID3D11BlendState> AccumulateBS;
	extern ComPtr<ID3D11BlendState> AlphaBS;

	// Graphics Pipeline States
	extern GraphicsPSO DefaultSolidPSO;
	extern GraphicsPSO DefaultWirePSO;
	extern GraphicsPSO StencilMaskPSO;
	extern GraphicsPSO ReflectSolidPSO;
	extern GraphicsPSO ReflectWirePSO;
	extern GraphicsPSO MirrorBlendSolidPSO;
	extern GraphicsPSO MirrorBlendWirePSO;
	extern GraphicsPSO SkyboxSolidPSO;
	extern GraphicsPSO SkyboxWirePSO;
	extern GraphicsPSO ReflectSkyboxSolidPSO;
	extern GraphicsPSO ReflectSkyboxWirePSO;
	extern GraphicsPSO NormalsPSO;
	extern GraphicsPSO DepthOnlyPSO;
	extern GraphicsPSO PostProcessingPSO;

	void InitCommonStates(ComPtr<ID3D11Device>& device);

	void SetPipelineState(ComPtr<ID3D11DeviceContext>& context, const GraphicsPSO& pso);

	// 내부적으로 InitCommonStates()에서 사용
	void InitSamplers(ComPtr<ID3D11Device>& device);
	void InitRasterizerStates(ComPtr<ID3D11Device>& device);
	void InitBlendStates(ComPtr<ID3D11Device>& device);
	void InitDepthStencilStates(ComPtr<ID3D11Device>& device);
	void InitPipelineStates(ComPtr<ID3D11Device>& device);
	void InitShaders(ComPtr<ID3D11Device>& device);

} // namespace Graphics
