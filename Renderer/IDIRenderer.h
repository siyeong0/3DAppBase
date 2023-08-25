#pragma once
#include "dllAPI.h"
#include "RenderContext.h"
#include "IDIMeshObject.h"
#include "RenderOption.h"
#include "Camera.h"
#include "InputProc.h"

constexpr int DEFAULT_VIEWPORT_WIDTH = 1920;
constexpr int DEFAULT_VIEWPORT_HEIGHT = 1080;
constexpr int DEFAULT_VIEWPORT_TOPLEFT_X = 0;
constexpr int DEFAULT_VIEWPORT_TOPLEFT_Y = 0;

class IDIRenderer;

extern "C" RENDERER_API IDIRenderer * GetRenderer();

class RENDERER_API IDIRenderer
{
public:
	IDIRenderer(HWND mainWindow);
	virtual ~IDIRenderer();

	virtual bool Initialize();
	virtual void Update(float dt);
	virtual void RenderBegin() = 0;
	void Render(const RenderContext& context);
	virtual void RenderEnd() = 0;
	virtual void Present() = 0;

	void RegistMesh(const std::string& name, IDIMeshObject* meshObj);
	void AddSpotLight(const Vector3& pos, const Vector3& dir, const Vector3& radiance, float radius, float strength);

	void InitCamera(Camera* camera);
	virtual void InitSkybox(std::wstring envFilename, std::wstring specularFilename, std::wstring irradianceFilename, std::wstring brdfFilename) = 0;

	void SetRenderOption(RenderOption& option);
	void SetViewport(int topLeftX, int topLeftY, int width, int height);
	virtual void Resize(int width, int height);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	RenderOption& GetRenderOption();
	bool& OptionDirty();
	int GetScreenWidth() const;
	int GetScreenHeight() const;
	int GetViewportTopLeftX() const;
	int GetViewportTopLeftY() const;
	int GetViewportWidth() const;
	int GetViewportHeight() const;

public:
// protected:
	virtual bool initDevice() = 0;
	virtual void setMainViewport() = 0;

	float aspectRatio() const;
	void updateNdc(int x, int y);

public:
// protected:
	// Object Buffers
	std::unordered_map<std::string, IDIMeshObject*> mMeshTable;
	std::vector<const RenderContext*> mInCameraFrustum;
	std::vector<const RenderContext*> mInLightFrustumArr[MAX_LIGHTS];

	// Global Constant Data
	GlobalConstantsData mGlobalConstData;
	GlobalConstantsData mGlobalMirrorConstData;
	int mNumLights = 0;

	// IO Utils
	Camera* mCamera;

	// Main Window
	HWND mMainWindow;

	int mViewportTopLeftX;
	int mViewportTopLeftY;
	int mViewportWidth;
	int mViewportHeight;

	double mViewportTopLeftXRatio;
	double mViewportTopLeftYRatio;
	double mViewportWidthRatio;
	double mViewportHeightRatio;

	float mCursorNdcX;
	float mCursorNdcY;

	// Render Options
	RenderOption mOption;
	bool mbOptionDirty = true;
};