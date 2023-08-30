#include "pch.h"
#include "GLRenderer.h"

GLRenderer::GLRenderer(const std::wstring& title)
	: IDIRenderer(title)
	, mMainWindow(nullptr)
{

}

GLRenderer::~GLRenderer()
{

}

bool GLRenderer::Initialize()
{
	IDIRenderer::Initialize();
	if (!initDevice())
	{
		return false;
	}

	return true;
}

void GLRenderer::Update(float dt)
{

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
	std::string title = std::string().assign(mTitle.begin(), mTitle.end());
	mMainWindow = glfwCreateWindow(mOption.Resolution.Width, mOption.Resolution.Height, title.c_str(), NULL, NULL);
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

bool GLRenderer::initGui()
{

	return true;
}

void GLRenderer::setMainViewport()
{
	glViewport(mViewportTopLeftX, mViewportTopLeftY, mViewportWidth, mViewportHeight);
}
