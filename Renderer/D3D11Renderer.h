#pragma once
#include "dllAPI.h"
#include "IDIRenderer.h"

#include "D3D11Utils.h"
#include "D3D11Common.h"
#include "D3D11Skybox.h"
#include "D3D11PostProcess.h"

class RENDERER_API D3D11Renderer : public IDIRenderer
{
public:
	D3D11Renderer(HWND mainWindow);
	virtual ~D3D11Renderer();

	virtual bool Initialize() override;
	virtual void Update(float dt) override;

	virtual void RenderBegin() override;
	virtual void RenderEnd() override;
	virtual void Present() override;

	virtual void Resize(int width, int height) override;

	virtual void InitSkybox(std::wstring envFilename, std::wstring specularFilename, std::wstring irradianceFilename, std::wstring brdfFilename) override;

// protected:
public:
	virtual bool initDevice() override;
	virtual void setMainViewport() override;
	void setShadowViewport();

	void createBuffers();
	void updateGlobalConstBuffer(float dt);
	void updateMirrorGlobalConstBuffer(const Matrix& reflect);
	void updateShadowGlobalConstBuffer(float dt);
	void setGlobalConstantBuffers(ComPtr<ID3D11Buffer>& globalConstBuf);
// pivate:
public:
	// D3D11
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mContext;
	ComPtr<IDXGISwapChain> mSwapChain;
	// Viewport
	D3D11_VIEWPORT mScreenViewport;
	// Global Constant
	ComPtr<ID3D11Buffer> mGlobalConstBuffer;
	ComPtr<ID3D11Buffer> mGlobalMirrorConstBuffer;
	// Render Target View and Shader Resource View
	ComPtr<ID3D11RenderTargetView> mBackBufferRTV;
	ComPtr<ID3D11Texture2D> mFloatBuffer;
	ComPtr<ID3D11Texture2D> mResolvedBuffer;
	ComPtr<ID3D11RenderTargetView> mFloatRTV;
	ComPtr<ID3D11RenderTargetView> mResolvedRTV;
	ComPtr<ID3D11ShaderResourceView> mFloatSRV;
	ComPtr<ID3D11ShaderResourceView> mResolvedSRV;
	// Depth Stencil View
	ComPtr<ID3D11DepthStencilView> mDefaultDSV;	// MSAA On
	// Depth buffer
	ComPtr<ID3D11Texture2D> mDepthOnlyBuffer;
	ComPtr<ID3D11DepthStencilView> mDepthOnlyDSV;	// MSAA Off
	ComPtr<ID3D11ShaderResourceView> mDepthOnlySRV;
	// Shadow
	int mShadowWidth = 1280;
	int mShadowHeight = 1280;
	ComPtr<ID3D11Texture2D> mShadowBuffers[MAX_LIGHTS]; // No MSAA
	ComPtr<ID3D11DepthStencilView> mShadowDSVs[MAX_LIGHTS];
	ComPtr<ID3D11ShaderResourceView> mShadowSRVs[MAX_LIGHTS];
	ComPtr<ID3D11Buffer> mShadowGlobalConstBuffers[MAX_LIGHTS];
	// Rasterizer State
	ComPtr<ID3D11RasterizerState> mSolidRS;
	ComPtr<ID3D11RasterizerState> mWireRS;
	ComPtr<ID3D11RasterizerState> mSolidCcwRS;
	ComPtr<ID3D11RasterizerState> mWireCcwRS;
	// Blend State
	ComPtr<ID3D11BlendState> mMirrorBS;
	// Skybox
	D3D11Skybox mCubeMapping;
	// PostProcess
	D3D11PostProcess mPostProcess;

	// Options
	UINT mNumQualityLevels = 0;
};