#include "pch.h"
#include "IDIRenderer.h"
#include "D3D11Utils.h"

#include <algorithm>
#include <directxtk/DDSTextureLoader.h>

using namespace std;

IDIRenderer* gRenderer = nullptr;

IDIRenderer* GetRenderer()
{
	return gRenderer;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	GetRenderer()->MsgProc(hWnd, msg, wParam, lParam);
	switch (msg)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
		{
			return 0;
		}
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);;
}

IDIRenderer::IDIRenderer(std::wstring title)
	: mTitle(title)
	, mViewportTopLeftX(DEFAULT_VIEWPORT_TOPLEFT_X)
	, mViewportTopLeftY(DEFAULT_VIEWPORT_TOPLEFT_Y)
	, mViewportWidth(DEFAULT_VIEWPORT_WIDTH)
	, mViewportHeight(DEFAULT_VIEWPORT_HEIGHT)
	, mViewportTopLeftXRatio(double(mViewportTopLeftX) / mOption.Resolution.Width)
	, mViewportTopLeftYRatio(double(mViewportTopLeftY) / mOption.Resolution.Height)
	, mViewportWidthRatio(double(mViewportWidth) / mOption.Resolution.Width)
	, mViewportHeightRatio(double(mViewportHeight) / mOption.Resolution.Height)
	, mGui(nullptr)
	, mCamera(nullptr)
	, mInputProc(nullptr)
	, mCursorNdcX(0.0f)
	, mCursorNdcY(0.0f)
{
	gRenderer = this;
}

IDIRenderer::~IDIRenderer()
{
	gRenderer = nullptr;

	mGui->OnDetach();

	for (auto& ptr : mMeshTable)
	{
		delete ptr.second;
	}
}

bool IDIRenderer::Initialize()
{
	mMeshTable.reserve(1024);
	mInCameraFrustum.reserve(1000000);

	return true;
}

void IDIRenderer::Update(float dt)
{
	mCamera->Update(dt);
	mInputProc->Update(dt);
}

void IDIRenderer::Render(const RenderContext& context)
{
	Vector3 center = context.TransformMatrix.Translation();
	const Vector3& ext = mMeshTable[context.MeshName]->GetExtents();
	BoundingBox box = BoundingBox(center, ext);
	if (mOption.bFrustumCulling && mCamera->IsVisible(box))
	{
		mInCameraFrustum.push_back(&context);
	}
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (true)
		{
			mInLightFrustumArr[i].push_back(&context);
		}
	}
}

void IDIRenderer::RegistMesh(const std::string& name, IDIMeshObject* meshObj)
{
	mMeshTable[name] = meshObj;
	auto temp = mMeshTable[name];
}

void IDIRenderer::AddSpotLight(const Vector3& pos, const Vector3& dir, const Vector3& radiance, float radius, float strength)
{
	mGlobalConstData.Lights[mNumLights].Position = pos;
	mGlobalConstData.Lights[mNumLights].Direction = dir;
	mGlobalConstData.Lights[mNumLights].Direction.Normalize();
	mGlobalConstData.Lights[mNumLights].Radiance = radiance;
	mGlobalConstData.Lights[mNumLights].Radius = radius;
	mGlobalConstData.Lights[mNumLights].SpotPower = strength;
	mGlobalConstData.Lights[mNumLights].Type = LIGHT_SPOT | LIGHT_SHADOW;
	mNumLights++;
}

void IDIRenderer::InitCamera(Camera* camera)
{
	mCamera = camera;
	mCamera->SetAspectRatio(aspectRatio());
}

void IDIRenderer::InitInputProc(InputProc* inputProc)
{
	mInputProc = inputProc;
}

void IDIRenderer::SetRenderOption(RenderOption& option)
{
	memcpy(&mOption, &option, sizeof(RenderOption));
}

void IDIRenderer::SetViewport(int topLeftX, int topLeftY, int width, int height)
{
	mViewportTopLeftX = topLeftX;
	mViewportTopLeftY = topLeftY;
	mViewportWidth = width;
	mViewportHeight = height;

	mViewportTopLeftXRatio = double(mViewportTopLeftX) / mOption.Resolution.Width;
	mViewportTopLeftYRatio = double(mViewportTopLeftY) / mOption.Resolution.Height;
	mViewportWidthRatio = double(mViewportWidth) / mOption.Resolution.Width;
	mViewportHeightRatio = double(mViewportHeight) / mOption.Resolution.Height;

	setMainViewport();
}

void IDIRenderer::Resize(int width, int height)
{
	mOption.Resolution.Width = width;
	mOption.Resolution.Height = height;

	mViewportTopLeftX = int(mViewportTopLeftXRatio * mOption.Resolution.Width);
	mViewportTopLeftY = int(mViewportTopLeftYRatio * mOption.Resolution.Height);
	mViewportWidth = int(mViewportWidthRatio * mOption.Resolution.Width);
	mViewportHeight = int(mViewportHeightRatio * mOption.Resolution.Height);

	mGui->Resize(width, height);
}

LRESULT IDIRenderer::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (mInputProc)
	{
		mInputProc->MsgProc(hwnd, msg, wParam, lParam);
	}
	switch (msg)
	{
	case WM_SIZE:	// Resizing window
		Resize(int(LOWORD(lParam)), int(HIWORD(lParam)));
		break;
	case WM_MOUSEMOVE:
		updateNdc(LOWORD(lParam), HIWORD(lParam));
		break;
	}

	return 0;
}

RenderOption& IDIRenderer::GetRenderOption() { return mOption; }
bool& IDIRenderer::OptionDirty() { return mbOptionDirty; }

int IDIRenderer::GetScreenWidth() const { return mOption.Resolution.Width; }
int IDIRenderer::GetScreenHeight() const { return mOption.Resolution.Height; }

int IDIRenderer::GetViewportTopLeftX() const { return mViewportTopLeftX; }
int IDIRenderer::GetViewportTopLeftY() const { return mViewportTopLeftY; }
int IDIRenderer::GetViewportWidth() const { return mViewportWidth; }
int IDIRenderer::GetViewportHeight() const { return mViewportHeight; }

float IDIRenderer::aspectRatio() const
{
	return float(mViewportWidth) / float(mViewportHeight);
}

void IDIRenderer::updateNdc(int x, int y)
{
	float ndcX = (x - mViewportTopLeftX) * 2.0f / (mViewportWidth)-1.0f;
	float ndcY = -(y - mViewportTopLeftY) * 2.0f / (mViewportHeight)+1.0f;
	ndcX = std::clamp(ndcX, -1.0f, 1.0f);
	ndcY = std::clamp(ndcY, -1.0f, 1.0f);

	mCursorNdcX = ndcX;
	mCursorNdcY = ndcY;
}