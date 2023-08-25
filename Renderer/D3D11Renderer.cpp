#include "pch.h"
#include "D3D11Renderer.h"

#include "D3D11MeshObject.h"
#include "D3D11Skybox.h"

D3D11Renderer::D3D11Renderer(HWND mainWindow)
	: IDIRenderer(mainWindow)
	, mScreenViewport(D3D11_VIEWPORT())
{

}

D3D11Renderer::~D3D11Renderer()
{

}

bool D3D11Renderer::Initialize()
{
	bool br = false;
	br = IDIRenderer::Initialize();

	D3D11MeshObject::Initialize(mDevice, mContext);
	D3D11Skybox::Initialize(mDevice, mContext);

	return br;
}

void D3D11Renderer::Update(float dt)
{
	IDIRenderer::Update(dt);

	updateGlobalConstBuffer(dt);
	updateShadowGlobalConstBuffer(dt);

	if (mbOptionDirty)
	{
		mGlobalConstData.IBLStrength = mOption.IBLStrength;

		mPostProcess.mCombineFilter.mConstData.Strength = mOption.BloomStrength;
		mPostProcess.mCombineFilter.mConstData.Option1 = mOption.Exposure;
		mPostProcess.mCombineFilter.mConstData.Option2 = mOption.Gamma;
		mPostProcess.mCombineFilter.UpdateConstantBuffers(mDevice, mContext);

		mbOptionDirty = false;
	}
}

void D3D11Renderer::RenderBegin()
{
	setMainViewport();

	// Set Sampler
	mContext->VSSetSamplers(0, UINT(D3D11Common::sampleStates.size()), D3D11Common::sampleStates.data());
	mContext->PSSetSamplers(0, UINT(D3D11Common::sampleStates.size()), D3D11Common::sampleStates.data());
	// Set Global Textures
	std::vector<ID3D11ShaderResourceView*> srvs =
	{
		mCubeMapping.mEnvSRV.Get(),
		mCubeMapping.mSpecularSRV.Get(),
		mCubeMapping.mIrradianceSRV.Get(),
		mCubeMapping.mBrdfSRV.Get(),
	};
	mContext->PSSetShaderResources(10, UINT(srvs.size()), srvs.data());
	// Clear Buffers
	mInCameraFrustum.clear();
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		mInLightFrustumArr[i].clear();
	}
}

void D3D11Renderer::RenderEnd()
{
	using namespace D3D11Common;
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	std::vector<ID3D11RenderTargetView*> rtvs = { mFloatRTV.Get() };

	//// Draw Depth
	//mContext->OMSetRenderTargets(1, mResolvedRTV.GetAddressOf(), mDepthOnlyDSV.Get());
	//mContext->ClearDepthStencilView(mDepthOnlyDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	//D3D11Common::SetPipelineState(mContext, DepthOnlyPSO);
	//setGlobalConstantBuffers(mGlobalConstBuffer);
	//for (auto& pModel : mInCameraFrustum)
	//{
	//	mMeshTable[pModel->MeshName]->Render(pModel->TransformMatrix);
	//}
	//mCubeMapping.Render();

	// Shadow Map
	setShadowViewport();
	SetPipelineState(mContext, DepthOnlyPSO);
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (mGlobalConstData.Lights[i].Type & LIGHT_SHADOW)
		{
			mContext->OMSetRenderTargets(0, NULL, mShadowDSVs[i].Get());
			mContext->ClearDepthStencilView(mShadowDSVs[i].Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			setGlobalConstantBuffers(mShadowGlobalConstBuffers[i]);
			for (auto& pModel : mInLightFrustumArr[i])
			{
				mMeshTable[pModel->MeshName]->Render(pModel->TransformMatrix);
			}
			mCubeMapping.Render();
		}
	}

	// Draw main
	setMainViewport();
	for (size_t i = 0; i < rtvs.size(); i++)
	{
		mContext->ClearRenderTargetView(rtvs[i], clearColor);
	}
	mContext->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(), mDefaultDSV.Get());

	std::vector<ID3D11ShaderResourceView*> shadowSRVs;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		shadowSRVs.push_back(mShadowSRVs[i].Get());
	}
	mContext->PSSetShaderResources(15, UINT(shadowSRVs.size()), shadowSRVs.data());

	mContext->ClearDepthStencilView(mDefaultDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	setGlobalConstantBuffers(mGlobalConstBuffer);

	for (auto& pModel : mInCameraFrustum)
	{
		D3D11Common::SetPipelineState(mContext, mOption.bWireFrame ? DefaultWirePSO : DefaultSolidPSO);
		mMeshTable[pModel->MeshName]->Render(pModel->TransformMatrix);
	}
	D3D11Common::SetPipelineState(mContext, mOption.bWireFrame ? SkyboxWirePSO : SkyboxSolidPSO);
	mCubeMapping.Render();

	// Post Process
	mContext->ResolveSubresource(mResolvedBuffer.Get(), 0, mFloatBuffer.Get(), 0, DXGI_FORMAT_R16G16B16A16_FLOAT);

	//mContext->PSSetShaderResources(14, 1, mCubeMapping.mBrdfSRV.GetAddressOf());
	D3D11Common::SetPipelineState(mContext, PostProcessingPSO);
	mContext->PSSetShaderResources(14, 1, mDepthOnlySRV.GetAddressOf());
	mPostProcess.Render(mContext);
}

void D3D11Renderer::Present()
{
	mSwapChain->Present(1, 0);
}

void D3D11Renderer::Resize(int width, int height)
{
	IDIRenderer::Resize(width, height);
	if (mSwapChain)
	{
		mViewportTopLeftX = int(mViewportTopLeftXRatio * mOption.Resolution.Width);
		mViewportTopLeftY = int(mViewportTopLeftYRatio * mOption.Resolution.Height);
		mViewportWidth = int(mViewportWidthRatio * mOption.Resolution.Width);
		mViewportHeight = int(mViewportHeightRatio * mOption.Resolution.Height);

		mBackBufferRTV.Reset();
		mSwapChain->ResizeBuffers(0, UINT(mOption.Resolution.Width), UINT(mOption.Resolution.Height), DXGI_FORMAT_UNKNOWN, 0);
		mCamera->SetAspectRatio(aspectRatio());
		createBuffers();
	}
}

void D3D11Renderer::InitSkybox(std::wstring envFilename, std::wstring specularFilename, std::wstring irradianceFilename, std::wstring brdfFilename)
{
	mCubeMapping.Initialize(envFilename, specularFilename, irradianceFilename, brdfFilename);
}

bool D3D11Renderer::initDevice()
{
	const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	const D3D_FEATURE_LEVEL featureLevels[2] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_9_3
	};
	D3D_FEATURE_LEVEL featureLevel;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = mOption.Resolution.Width;
	sd.BufferDesc.Height = mOption.Resolution.Height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferCount = 2;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = mMainWindow;
	sd.Windowed = TRUE;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
	// sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //ImGui font issue
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1; // _FLIP_ doesn't support MSAA
	sd.SampleDesc.Quality = 0;

	ThrowIfFailed(D3D11CreateDeviceAndSwapChain(
		0, driverType, 0, createDeviceFlags, featureLevels, 1,
		D3D11_SDK_VERSION, &sd, mSwapChain.GetAddressOf(),
		mDevice.GetAddressOf(), &featureLevel, mContext.GetAddressOf()));

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		std::cout << "D3D Feature Level 11 unsupported." << std::endl;
		return false;
	}

	setMainViewport();
	createBuffers();

	// Init D3D11 Common values
	D3D11Common::InitCommonStates(mDevice);

	return true;
}

void D3D11Renderer::createBuffers()
{
	// Rasterization -> float/depthBuffer(MSAA) -> resolved -> backBuffer
	{
		// BackBuffer requires only RTV not SRV
		ComPtr<ID3D11Texture2D> backBuffer;
		ThrowIfFailed(mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));
		ThrowIfFailed(mDevice->CreateRenderTargetView(backBuffer.Get(), NULL, mBackBufferRTV.GetAddressOf()));

		// FLOAT MSAA RenderTargetView/ShaderResourceView
		ThrowIfFailed(mDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &mNumQualityLevels));

		D3D11_TEXTURE2D_DESC desc;
		backBuffer->GetDesc(&desc);
		desc.MipLevels = desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스춰로부터 복사 가능
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = 0;
		if (mOption.AA == eAntialiasing::MSAA && mNumQualityLevels)
		{
			desc.SampleDesc.Count = 4;
			desc.SampleDesc.Quality = mNumQualityLevels - 1;
		}
		else
		{
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}

		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mFloatBuffer.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateShaderResourceView(mFloatBuffer.Get(), NULL, mFloatSRV.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateRenderTargetView(mFloatBuffer.Get(), NULL, mFloatRTV.GetAddressOf()));

		// FLOAT MSAA SRV/RTV
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mResolvedBuffer.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateShaderResourceView(mResolvedBuffer.Get(), NULL, mResolvedSRV.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateRenderTargetView(mResolvedBuffer.Get(), NULL, mResolvedRTV.GetAddressOf()));
	}

	// Depth Stencil Buffer
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = mOption.Resolution.Width;
		desc.Height = mOption.Resolution.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		if (mOption.AA == eAntialiasing::MSAA && mNumQualityLevels > 0)
		{
			desc.SampleDesc.Count = 4;
			desc.SampleDesc.Quality = mNumQualityLevels - 1;
		}
		else
		{
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, depthStencilBuffer.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, mDefaultDSV.GetAddressOf()));

		// Depth Only Buffer
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mDepthOnlyBuffer.GetAddressOf()));
		//Shadow Map
		desc.Width = mShadowWidth;
		desc.Height = mShadowHeight;
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mShadowBuffers[i].GetAddressOf()));
		}
		// Depth Only DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroMemory(&dsvDesc, sizeof(dsvDesc));
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		ThrowIfFailed(mDevice->CreateDepthStencilView(mDepthOnlyBuffer.Get(), &dsvDesc, mDepthOnlyDSV.GetAddressOf()));
		// Shadow Map
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			ThrowIfFailed(mDevice->CreateDepthStencilView(mShadowBuffers[i].Get(), &dsvDesc, mShadowDSVs[i].GetAddressOf()));
		}
		// Depth Only SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		ThrowIfFailed(mDevice->CreateShaderResourceView(mDepthOnlyBuffer.Get(), &srvDesc, mDepthOnlySRV.GetAddressOf()));
		// Shadow Map
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			ThrowIfFailed(mDevice->CreateShaderResourceView(mShadowBuffers[i].Get(), &srvDesc, mShadowSRVs[i].GetAddressOf()));
		}
	}
	mPostProcess.Initialize(mDevice, mContext, { mResolvedSRV }, { mBackBufferRTV }, mOption.Resolution.Width, mOption.Resolution.Height, 4);

	// Create GlobalConstBuffer;
	mGlobalConstData.EyeWorld = { 0,0,0 };
	mGlobalConstData.View = Matrix();
	mGlobalConstData.Proj = Matrix();
	mGlobalConstData.InvProj = Matrix();
	mGlobalConstData.ViewProj = Matrix();
	mGlobalConstData.InvViewProj = Matrix();
	mGlobalConstData.InvView = Matrix();

	D3D11Utils::CreateConstBuffer(mDevice, mGlobalConstData, mGlobalConstBuffer);
	D3D11Utils::CreateConstBuffer(mDevice, mGlobalMirrorConstData, mGlobalMirrorConstBuffer);
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		D3D11Utils::CreateConstBuffer(mDevice, mShadowGlobalConstantDatas[i], mShadowGlobalConstBuffers[i]);
	}
}

void D3D11Renderer::setMainViewport()
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
	mScreenViewport.TopLeftX = float(mViewportTopLeftX);
	mScreenViewport.TopLeftY = float(mViewportTopLeftY);
	mScreenViewport.Width = float(mViewportWidth);
	mScreenViewport.Height = float(mViewportHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mContext->RSSetViewports(1, &mScreenViewport);
}

void D3D11Renderer::setShadowViewport()
{
	D3D11_VIEWPORT shadowViewport;
	ZeroMemory(&shadowViewport, sizeof(D3D11_VIEWPORT));
	shadowViewport.TopLeftX = 0;
	shadowViewport.TopLeftY = 0;
	shadowViewport.Width = float(mShadowWidth);
	shadowViewport.Height = float(mShadowHeight);
	shadowViewport.MinDepth = 0.0f;
	shadowViewport.MaxDepth = 1.0f;

	mContext->RSSetViewports(1, &shadowViewport);
}

void D3D11Renderer::updateGlobalConstBuffer(float dt)
{
	Matrix viewRow = mCamera->ViewMatrix();
	Matrix projRow = mCamera->ProjMatrix();
	Vector3 eyeWorld = mCamera->GetPosition();

	mGlobalConstData.GlobalTime += dt;
	mGlobalConstData.EyeWorld = eyeWorld;
	mGlobalConstData.View = viewRow.Transpose();
	mGlobalConstData.Proj = projRow.Transpose();
	mGlobalConstData.InvView = viewRow.Invert().Transpose();
	mGlobalConstData.InvProj = projRow.Invert().Transpose();
	mGlobalConstData.ViewProj = (viewRow * projRow).Transpose();
	mGlobalConstData.InvViewProj = (viewRow * projRow).Invert().Transpose();

	D3D11Utils::UpdateBuffer(mDevice, mContext, mGlobalConstData, mGlobalConstBuffer);
}

void D3D11Renderer::updateMirrorGlobalConstBuffer(const Matrix& reflect)
{
	Matrix viewRow = mCamera->ViewMatrix();
	Matrix projRow = mCamera->ProjMatrix();
	Vector3 eyeWorld = mCamera->GetPosition();

	memcpy(&mGlobalMirrorConstData, &mGlobalConstData, sizeof(GlobalConstantsData));
	mGlobalMirrorConstData.View = (reflect * viewRow).Transpose();
	mGlobalMirrorConstData.ViewProj = (reflect * viewRow * projRow).Transpose();
	mGlobalMirrorConstData.InvViewProj = mGlobalMirrorConstData.ViewProj.Invert();

	D3D11Utils::UpdateBuffer(mDevice, mContext, mGlobalMirrorConstData, mGlobalMirrorConstBuffer);
}

void D3D11Renderer::updateShadowGlobalConstBuffer(float dt)
{
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		const auto& light = mGlobalConstData.Lights[i];
		if (light.Type & LIGHT_SHADOW)
		{
			Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
			if (abs(up.Dot(light.Direction) + 1.0f) < 1e-5)
			{
				up = Vector3(1.0f, 0.0f, 0.0f);
			}

			Matrix lightViewRow = DirectX::XMMatrixLookToLH(light.Position, light.Direction, up);
			Matrix lightProjRow = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(120.0f), 1.0f, 0.01f, 1000.0f);

			mShadowGlobalConstantDatas[i].EyeWorld = light.Position;
			mShadowGlobalConstantDatas[i].View = lightViewRow.Transpose();
			mShadowGlobalConstantDatas[i].Proj = lightProjRow.Transpose();
			mShadowGlobalConstantDatas[i].InvView = lightViewRow.Invert().Transpose();
			mShadowGlobalConstantDatas[i].InvProj = lightProjRow.Invert().Transpose();
			mShadowGlobalConstantDatas[i].ViewProj = (lightViewRow * lightProjRow).Transpose();
			mShadowGlobalConstantDatas[i].InvViewProj = (lightViewRow * lightProjRow).Invert().Transpose();
			D3D11Utils::UpdateBuffer(mDevice, mContext, mShadowGlobalConstantDatas[i], mShadowGlobalConstBuffers[i]);

			mGlobalConstData.Lights[i].ViewProj = mShadowGlobalConstantDatas[i].ViewProj;
			mGlobalConstData.Lights[i].InvProj = mShadowGlobalConstantDatas[i].InvProj;
		}
	}
}

void D3D11Renderer::setGlobalConstantBuffers(ComPtr<ID3D11Buffer>& globalConstBuf)
{
	mContext->VSSetConstantBuffers(0, 1, globalConstBuf.GetAddressOf());
	mContext->PSSetConstantBuffers(0, 1, globalConstBuf.GetAddressOf());
	mContext->GSSetConstantBuffers(0, 1, globalConstBuf.GetAddressOf());
}