#include "pch.h"
#include "GLRenderer.h"

GLRenderer::GLRenderer(const std::string& title)
	: IDIRenderer(title)
	, mMainWindow(nullptr)
{

}

GLRenderer::~GLRenderer()
{

}

bool GLRenderer::Initialize()
{
	bool br = false;
	br = IDIRenderer::Initialize();

	return br;
}

void GLRenderer::Update(float dt)
{
	IDIRenderer::Update(dt);

	if (mbOptionDirty)
	{
		mGlobalConstData.IBLStrength = mOption.IBLStrength;
		mbOptionDirty = false;
	}
}

void GLRenderer::RenderBegin()
{

}

void GLRenderer::RenderEnd()
{

}

void GLRenderer::Present()
{

}

void GLRenderer::Resize(int width, int height)
{
	IDIRenderer::Resize(width, height);
	setMainViewport();
}

void GLRenderer::InitSkybox(std::wstring envFilename, std::wstring specularFilename, std::wstring irradianceFilename, std::wstring brdfFilename)
{

}

void GLRenderer::Terminate()
{

}

void GLRenderer::SetCursorPosNdc(float x, float y)
{

}

bool GLRenderer::initDevice()
{
	// Init libarary
	if (!glfwInit())
	{
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Init Window
	mMainWindow = glfwCreateWindow(mOption.Resolution.Width, mOption.Resolution.Height, mTitle.c_str(), NULL, NULL);
	if (!mMainWindow)
	{
		glfwTerminate();
		return false;
	}
	// Init Context
	glfwMakeContextCurrent(mMainWindow);
	if (glewInit() != GLEW_OK)
	{
		return false;
	}

	return true;
}

void GLRenderer::setMainViewport()
{
	glViewport(mViewportTopLeftX, mViewportTopLeftY, mViewportWidth, mViewportHeight);
}
