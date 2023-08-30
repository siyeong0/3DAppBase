#pragma once
#include "dllAPI.h"
#include "IDIRenderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class RENDERER_API GLRenderer : public IDIRenderer
{
public:
	GLRenderer(const std::wstring& title);
	virtual ~GLRenderer();

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
	virtual bool initGui() override;
	virtual void setMainViewport() override;

// pivate:
public:
	GLFWwindow* mMainWindow;
};