#include "ModelViewer.h"
#include "EditorGui.h"
#include "InputProcFuncs.h"
#include "D3D11MeshObject.h"

ModelViewer* gModelViewer = nullptr;

extern ModelViewer* GetModelViewer()
{
	return gModelViewer;
}

ModelViewer::ModelViewer(eDevice device)
	: meDev(device)
	, mGui(nullptr)
	, mMainCamera(nullptr)
{
	gModelViewer = this;
}
ModelViewer::~ModelViewer()
{
	gModelViewer = nullptr;

	for (auto& ptr : mFrames)
	{
		delete ptr;
	}
}

bool ModelViewer::Initialize()
{
	// Init Renderer
	{
		std::wstring title = L"Model Viewer";
		switch (meDev)
		{
		case eDevice::DX11:
			mRenderer = make_unique<D3D11Renderer>(title);
			break;
		case eDevice::GL:
			mRenderer = make_unique<GLRenderer>(title);
			break;
		default:
			assert(false);
		}
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

	//// Init MainModel
	//{
	//	auto meshes = std::vector{ GeometryGenerator::MakeBox(0.2f) };
	//	mRenderer->RegistMesh("box", new D3D11MeshObject(meshes));
	//	mMainModel = RenderContext("box");
	//}

	//// Init Floor
	//{
	//	auto mesh = GeometryGenerator::MakeSquare(100.0f, { 100.0f, 100.0f });
	//	std::string path = "../Assets/Textures/PBR/stringy-marble-ue/";
	//	mesh.AlbedoTextureFilename = path + "stringy_marble_albedo.png";
	//	mesh.EmissiveTextureFilename = "";
	//	mesh.AoTextureFilename = path + "stringy_marble_ao.png";
	//	mesh.MetallicTextureFilename = path + "stringy_marble_metallic.png";
	//	mesh.NormalTextureFilename = path + "stringy_marble_normal.png";
	//	mesh.RoughnessTextureFilename = path + "stringy_marble_roughness.png";

	//	mRenderer->RegistMesh("plane", new D3D11MeshObject({ mesh }));
	//	mFloor = RenderContext("plane");
	//	mFloor.TransformMatrix = Matrix::CreateFromQuaternion({ 0.7071067f, 0, 0, 0.7071069f }) * Matrix::CreateTranslation(0.0f, FLOOR_Y, 0.0f);
	//}

	//// Init Lights
	//{
	//	Vector3 pos = { 1.0f, 1.5f, 0.0f };
	//	mRenderer->AddSpotLight(pos, { 0.0f, -1.0f, 0 }, { 0.5f, 0.5f, 0.5f }, 0.03f, 5.0f);
	//	Vector3 pos2 = { 0.0f, 1.5f, 1.0f };
	//	mRenderer->AddSpotLight(pos2, { 1.0f, -1.0f, 0 }, { 0.7f, 0.7f, 0.7f }, 0.05f, 10.0f);

	//	auto meshes = std::vector{ GeometryGenerator::MakeSphere(0.03f, 3,3) };
	//	mRenderer->RegistMesh("sphere", new D3D11MeshObject(meshes));
	//	mExtras.push_back(RenderContext("sphere", pos));
	//	mExtras.push_back(RenderContext("sphere", pos2));
	//}

	//// Init Camera
	//{
	//	mMainCamera = new Camera({ 0.0f, 0.0f, -2.0f }, { 0,1,0 }, 0, 0);
	//	mRenderer->InitCamera(mMainCamera);
	//}

	//// Init Gui
	//{
	//	mGui = GetGui();
	//	IFrame* f = nullptr;

	//	f = new EditorGui::MenuBar();
	//	mFrames.push_back(f);
	//	mGui->Regist("menu", f);

	//	f = new EditorGui::LeftFrame();
	//	mFrames.push_back(f);
	//	mGui->Regist("left", f);

	//	f = new EditorGui::RightFrame();
	//	mFrames.push_back(f);
	//	mGui->Regist("right", f);

	//	f = new EditorGui::BottomFrame();
	//	mFrames.push_back(f);
	//	mGui->Regist("bottom", f);

	//	ImGui::SetCurrentContext(mGui->Context());
	//	mRenderer->SetViewport(EditorGui::GetViewportTopLeftX(), EditorGui::GetViewportTopLeftY(), EditorGui::GetViewportWidth(), EditorGui::GetViewportHeight());
	//}

	//// Init InputProc
	//{
	//	// First person view
	//	mInputProc['F'].Down = InputProcFuncs::SetFirstPersonView;
	//	// WASD Camera move
	//	mInputProc['W'].Keep = InputProcFuncs::MoveForward;
	//	mInputProc['A'].Keep = InputProcFuncs::MoveLeft;
	//	mInputProc['S'].Keep = InputProcFuncs::MoveBackward;
	//	mInputProc['D'].Keep = InputProcFuncs::MoveRight;
	//	mInputProc[' '].Keep = InputProcFuncs::MoveUp;
	//	mInputProc[VK_SHIFT].Keep = InputProcFuncs::MoveDown;
	//	// Window destroy
	//	mInputProc[27].Up = InputProcFuncs::Terminate;

	//	// Mouse Input
	//	mInputProc[MOUSE_MOVE].bKeep = true;
	//	mInputProc[MOUSE_MOVE].Keep = InputProcFuncs::MoveView;

	//	mInputProc[MOUSE_LEFT].Down = InputProcFuncs::DragBegin;
	//	mInputProc[MOUSE_LEFT].Keep = InputProcFuncs::DragObject;
	//	mInputProc[MOUSE_LEFT].Up = InputProcFuncs::DragEnd;
	//	mInputProc[MOUSE_RIGHT].Down = InputProcFuncs::RotateBegin;
	//	mInputProc[MOUSE_RIGHT].Keep = InputProcFuncs::RotateObject;
	//	mInputProc[MOUSE_RIGHT].Up = InputProcFuncs::RotateEnd;

	//	mRenderer->InitInputProc(&mInputProc);
	//}

	return true;
}

void ModelViewer::Run()
{
	IDIGui* mGui = GetGui();
	MSG msg = { 0 };
	while (WM_QUIT != msg.message || mbLoopExit == true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//// Update
			////float dt = ImGui::GetIO().DeltaTime;
			//float dt = 0.016f;
			//mInputProc.Update(dt);
			//mRenderer->Update(dt);

			//// Update Model position
			//Vector3 translation = mMainModel.TransformMatrix.Translation();
			//float minY = translation.y - mRenderer->mMeshTable[mMainModel.MeshName]->mExtents.y;
			//if (minY < FLOOR_Y)
			//{
			//	float yToUp = FLOOR_Y - minY;
			//	mMainModel.TransformMatrix.Translation(Vector3(0.0f));
			//	mMainModel.TransformMatrix = mMainModel.TransformMatrix * Matrix::CreateTranslation(Vector3(0, yToUp, 0) + translation);
			//}

			// 3D Rendering
			mRenderer->RenderBegin();

			// Remder objects
			//mRenderer->Render(mMainModel);
			//mRenderer->Render(mFloor);
			for (auto& e : mExtras)
			{
				mRenderer->Render(e);
			}

			mRenderer->RenderEnd();

			// Render GUI
			//mGui->RenderBegin();
			//mGui->Render("menu");
			//mGui->Render("left");
			//mGui->Render("right");
			//mGui->Render("bottom");
			//mGui->RenderEnd();

			mRenderer->Present();
		}
	}
}

Camera& ModelViewer::GetCamera() const { return *mMainCamera; }
RenderContext& ModelViewer::GetModel() { return mMainModel; }
InputProc& ModelViewer::GetInputProc() { return mInputProc; }

void ModelViewer::ExitLoop() { mbLoopExit = true; }