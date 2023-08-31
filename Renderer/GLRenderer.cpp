#include "pch.h"
#include "GLRenderer.h"
#include "GLUtils.h"
#include "ImageReader.h"

GLRenderer::GLRenderer(const std::wstring& title)
	: IDIRenderer(title)
	, mWindow(nullptr)
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
	IDIRenderer::Update(dt);

	static float z = 0.0f;
	z += 0.01f;
	mWorld = Matrix::CreateRotationY(z) * Matrix::CreateTranslation({0,0,z});
	mGlobalConstData.ViewProj = (mCamera->ViewMatrix() * mCamera->ProjMatrix());

	glUniformMatrix4fv(mMatWorld, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&mWorld));
	glUniformMatrix4fv(mMatVP, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&mGlobalConstData.ViewProj));

}

void GLRenderer::RenderBegin()
{
	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glUniform1i(glGetUniformLocation(mProgram, "albedo"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	// Draw
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	glDisableVertexAttribArray(0);
}

void GLRenderer::RenderEnd()
{
	glFlush();
}

void GLRenderer::Present()
{
	glfwSwapBuffers(mWindow);
}

void GLRenderer::Resize(int width, int height)
{
	IDIRenderer::Resize(width, height);
	setMainViewport();
}

void GLRenderer::InitSkybox(std::wstring envFilename, std::wstring specularFilename, std::wstring irradianceFilename, std::wstring brdfFilename)
{

}

void GLRenderer::SetCursorPosition(int x, int y)
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
	mWindow = glfwCreateWindow(mOption.Resolution.Width, mOption.Resolution.Height, title.c_str(), NULL, NULL);
	if (!mWindow)
	{
		glfwTerminate();
		return false;
	}
	// Init Context
	glfwMakeContextCurrent(mWindow);
	if (glewInit() != GLEW_OK)
	{
		return false;
	}
	// Z-Buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Vertex Buffer
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

	MeshData meshData = GeometryGenerator::MakeBox();
	glBufferData(GL_ARRAY_BUFFER, 11 * sizeof(float) * meshData.Vertices.size(), meshData.Vertices.data(), GL_STATIC_DRAW);
	// Index Buffer
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * meshData.Indices.size(), meshData.Indices.data(), GL_STATIC_DRAW);

	// Shaders
	mProgram = GLUtils::CreateProgram("../Renderer/Shaders/BasicVS.glsl", "../Renderer/Shaders/BasicPS.glsl");
	glUseProgram(mProgram);

	mMatWorld = glGetUniformLocation(mProgram, "world");
	mMatVP = glGetUniformLocation(mProgram, "viewProj");

	std::vector<uint8_t> texData;
	int width, height;
	ReadImage("../Assets/Textures/PBR/stringy-marble-ue/stringy_marble_albedo.png", texData, width, height);

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData.data());
	glGenerateMipmap(GL_TEXTURE_2D);

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
