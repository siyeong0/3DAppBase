#include "ModelViewer.h"
#include "EditorGui.h"
#include "InputProcFuncs.h"
#include "D3D11MeshObject.h"

ModelViewer* gModelViewer = nullptr;

extern ModelViewer* GetModelViewer()
{
	return gModelViewer;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	GetRenderer()->MsgProc(hWnd, msg, wParam, lParam);
	GetModelViewer()->GetGui()->MsgProc(hWnd, msg, wParam, lParam);
	GetModelViewer()->GetInputProc().MsgProc(hWnd, msg, wParam, lParam);

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

ModelViewer::ModelViewer()
	: mWindow(0)
	, mGui(nullptr)
	, mMainCamera(nullptr)
{
	gModelViewer = this;
}

bool ModelViewer::Initialize()
{
	// Init Window
	{
		WNDCLASSEX wc = { sizeof(WNDCLASSEX),
						 CS_CLASSDC,
						 WndProc,
						 0L,
						 0L,
						 GetModuleHandle(NULL),
						 NULL,
						 NULL,
						 NULL,
						 NULL,
						 L"Model Viewer", // lpszClassName, L-string
						 NULL };

		if (!RegisterClassEx(&wc))
		{
			std::cout << "RegisterClassEx() failed." << std::endl;
			return false;
		}

		RECT wr = { 0, 0, DEFAULT_RESOLUTION.Width, DEFAULT_RESOLUTION.Height };
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
		mWindow = CreateWindow(wc.lpszClassName, L"Model Viewer",
			WS_OVERLAPPEDWINDOW,
			100, // 윈도우 좌측 상단의 x 좌표
			100, // 윈도우 좌측 상단의 y 좌표
			wr.right - wr.left, // 윈도우 가로 방향 해상도
			wr.bottom - wr.top, // 윈도우 세로 방향 해상도
			NULL, NULL, wc.hInstance, NULL);
		if (!mWindow)
		{
			std::cout << "CreateWindow() failed." << std::endl;
			abort();
		}
	}
	// Init Renderer
	{
		mRenderer = make_unique<D3D11Renderer>(mWindow);
		if (!mRenderer->Initialize())
		{
			std::cout << "Initialization failed." << std::endl;
			return false;
		}

		// Init CubeMap
		mRenderer->InitSkybox(
			L"../Assets/Cubemaps/Sky/clear_pureskyEnvHDR.dds",
			L"../Assets/Cubemaps/Sky/clear_pureskySpecularHDR.dds",
			L"../Assets/Cubemaps/Sky/clear_pureskyDiffuseHDR.dds",
			L"../Assets/Cubemaps/Sky/clear_pureskyBrdf.dds");
	}

	// Init MainModel
	{
		auto meshes = std::vector{ GeometryGenerator::MakeBox(0.2f) };
		mRenderer->RegistMesh("box", new D3D11MeshObject(meshes));
		mMainModel = RenderContext("box");
	}
	// Init Floor
	{
		auto mesh = GeometryGenerator::MakeSquare(100.0f, { 100.0f, 100.0f });
		std::string path = "../Assets/Textures/PBR/stringy-marble-ue/";
		mesh.AlbedoTextureFilename = path + "stringy_marble_albedo.png";
		mesh.EmissiveTextureFilename = "";
		mesh.AoTextureFilename = path + "stringy_marble_ao.png";
		mesh.MetallicTextureFilename = path + "stringy_marble_metallic.png";
		mesh.NormalTextureFilename = path + "stringy_marble_normal.png";
		mesh.RoughnessTextureFilename = path + "stringy_marble_roughness.png";

		mRenderer->RegistMesh("plane", new D3D11MeshObject({ mesh }));
		mFloor = RenderContext("plane");
		mFloor.TransformMatrix = Matrix::CreateFromQuaternion({ 0.7071067f, 0, 0, 0.7071069f }) * Matrix::CreateTranslation(0.0f, FLOOR_Y, 0.0f);
	}
	
	// Init Lights
	{
		Vector3 pos = { 1,1.5,0 };
		mRenderer->AddSpotLight(pos, { 0.0f,-1.0f,0 }, { 0.5f, 0.5f, 0.5f }, 0.4f, 5.0f);

		auto meshes = std::vector{ GeometryGenerator::MakeSphere(0.03f, 3,3) };
		mRenderer->RegistMesh("sphere", new D3D11MeshObject(meshes));
		mExtras.push_back(RenderContext("sphere", pos));
	}

	// Init Camera
	{
		mMainCamera = new Camera({ 0.0f, 0.0f, -2.0f }, { 0,1,0 }, 0, 0);
		mRenderer->InitCamera(mMainCamera);
	}

	// Init GUI
	{
		mGui = new D3D11Gui(mWindow, mRenderer->mDevice, mRenderer->mContext);
		mGui->Assgin(mRenderer.get());
		mGui->OnAttach();
		ImGui::SetCurrentContext(mGui->Context());
		EditorGui::Assign(mGui);
		mRenderer->SetViewport(EditorGui::GetViewportTopLeftX(), EditorGui::GetViewportTopLeftY(), EditorGui::GetViewportWidth(), EditorGui::GetViewportHeight());
	}

	// Init InputProc
	{
		// First person view
		mInputProc['F'].Down = InputProcFuncs::SetFirstPersonView;
		// WASD Camera move
		mInputProc['W'].Keep = InputProcFuncs::MoveForward;
		mInputProc['A'].Keep = InputProcFuncs::MoveLeft;
		mInputProc['S'].Keep = InputProcFuncs::MoveBackward;
		mInputProc['D'].Keep = InputProcFuncs::MoveRight;
		mInputProc[' '].Keep = InputProcFuncs::MoveUp;
		mInputProc[VK_SHIFT].Keep = InputProcFuncs::MoveDown;
		// Window destroy
		mInputProc[27].Up = InputProcFuncs::Terminate;

		// Mouse Input
		mInputProc[MOUSE_MOVE].bKeep = true;
		mInputProc[MOUSE_MOVE].Keep = InputProcFuncs::MoveView;

		mInputProc[MOUSE_LEFT].Down = InputProcFuncs::DragBegin;
		mInputProc[MOUSE_LEFT].Keep = InputProcFuncs::DragObject;
		mInputProc[MOUSE_LEFT].Up = InputProcFuncs::DragEnd;
		mInputProc[MOUSE_RIGHT].Down = InputProcFuncs::RotateBegin;
		mInputProc[MOUSE_RIGHT].Keep = InputProcFuncs::RotateObject;
		mInputProc[MOUSE_RIGHT].Up = InputProcFuncs::RotateEnd;
	}

	return true;
}

void ModelViewer::Run()
{
	ShowWindow(mWindow, SW_SHOWDEFAULT);
	UpdateWindow(mWindow);

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update
			float dt = ImGui::GetIO().DeltaTime;
			mInputProc.Update(dt);
			mRenderer->Update(dt);

			// Update Model position
			Vector3 translation = mMainModel.TransformMatrix.Translation();
			float minY = translation.y - mRenderer->mMeshTable[mMainModel.MeshName]->mExtents.y;
			if (minY < FLOOR_Y)
			{
				float yToUp = FLOOR_Y - minY;
				mMainModel.TransformMatrix.Translation(Vector3(0.0f));
				mMainModel.TransformMatrix = mMainModel.TransformMatrix * Matrix::CreateTranslation(Vector3(0, yToUp, 0) + translation);
			}

			// 3D Rendering
			mRenderer->RenderBegin();

			// Remder objects
			mRenderer->Render(mMainModel);
			mRenderer->Render(mFloor);
			for (auto& e : mExtras)
			{
				mRenderer->Render(e);
			}

			mRenderer->RenderEnd();

			// Render GUI
			mGui->RenderBegin();
			EditorGui::DrawTopMenuBar();
			EditorGui::DrawLeftFrame();
			EditorGui::DrawRightFrame();
			EditorGui::DrawBottomFrame();
			mGui->RenderEnd();

			mRenderer->Present();
		}
	}

	mGui->OnDetach();
	DestroyWindow(mWindow);
}

HWND ModelViewer::GetWindow() const { return mWindow; }
IDIGui* ModelViewer::GetGui() const { return mGui; }
Camera& ModelViewer::GetCamera() const { return *mMainCamera; }
RenderContext& ModelViewer::GetModel() { return mMainModel; }
InputProc& ModelViewer::GetInputProc() { return mInputProc; }