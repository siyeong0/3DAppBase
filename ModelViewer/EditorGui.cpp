#include "EditorGui.h"
#include "GuiUtils.h"
#include "Renderer.h"
#include "D3D11MeshObject.h"
#include "ModelViewer.h"
#include <string>
#include <vector>
#include <filesystem>
#include <regex>

using namespace std;
using filesystem::directory_iterator;

static constexpr int UNIT_SCREEN_WIDTH = 1920;
static constexpr int UNIT_SCREEN_HEIGHT = 1080;

static constexpr int FIXED_TOP_MENUBAR_HEIGTH = 19;	// Fixed value, Always sub to BottomFrameHeight
static constexpr int DEFAULT_LEFT_FRAME_WIDTH = 240;
static constexpr int DEFAULT_LEFT_FRAME_HEIGHT = 720;
static constexpr int DEFAULT_RIGHT_FRAME_WIDTH = 400;
static constexpr int DEFAULT_RIGHT_FRAME_HEIGHT = 1080;
static constexpr int DEFAULT_BOTTOM_FRAME_WIDTH = 1520;
static constexpr int DEFAULT_BOTTOM_FRAME_HEIGHT = 360 - FIXED_TOP_MENUBAR_HEIGTH;

static constexpr double mLeftFrameWidthRatio = double(DEFAULT_LEFT_FRAME_WIDTH) / UNIT_SCREEN_WIDTH;
static constexpr double mLeftFrameHeightRatio = double(DEFAULT_LEFT_FRAME_HEIGHT) / UNIT_SCREEN_HEIGHT;
static constexpr double mRightFrameWidthRatio = double(DEFAULT_RIGHT_FRAME_WIDTH) / UNIT_SCREEN_WIDTH;
static constexpr double mRightFrameHeightRatio = double(DEFAULT_RIGHT_FRAME_HEIGHT) / UNIT_SCREEN_HEIGHT;
static constexpr double mBottomFrameWidthRatio = double(DEFAULT_BOTTOM_FRAME_WIDTH) / UNIT_SCREEN_WIDTH;
static constexpr double mBottomFrameHeightRatio = double(DEFAULT_BOTTOM_FRAME_HEIGHT) / UNIT_SCREEN_HEIGHT;

static int mScreenWidth;
static int mScreenHeight;

static int mLeftFrameWidth;
static int mLeftFrameHeight;
static int mRightFrameWidth;
static int mRightFrameHeight;
static int mBottomFrameWidth;
static int mBottomFrameHeight;

static void updateLengths() 
{
	mLeftFrameWidth = int(mLeftFrameWidthRatio * mScreenWidth);
	mLeftFrameHeight = int(mLeftFrameHeightRatio * mScreenHeight);
	mRightFrameWidth = int(mRightFrameWidthRatio * mScreenWidth);
	mRightFrameHeight = int(mRightFrameHeightRatio * mScreenHeight);
	mBottomFrameWidth = int(mBottomFrameWidthRatio * mScreenWidth);
	mBottomFrameHeight = int(mBottomFrameHeightRatio * mScreenHeight);
}

void EditorGui::Assign(int w, int h)
{
	mScreenWidth = w;
	mScreenHeight = h;

	updateLengths();
}

void EditorGui::DrawTopMenuBar()
{
	wstring path;
	wstring name;

	updateLengths();

	ImGui::BeginMainMenuBar();
	ImGui::SetWindowSize(ImVec2(0.f, float(mScreenWidth - mRightFrameWidth)));  // Right frame overdraw menubar

	// Menu "File"
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::BeginMenu("New"))
		{
			if (ImGui::MenuItem("Model"))
			{
				if (BrowseFile(path, name))
				{
					path.assign(path.begin(), path.end() - name.length());
					auto meshes = GeometryGenerator::ReadFromFile(toString(path), toString(name));
					GetRenderer()->RegistMesh(toString(name), new D3D11MeshObject(meshes));
					GetModelViewer()->GetModel() = RenderContext(toString(name));
				}
			}

			if (ImGui::MenuItem("Skybox"))
			{
				if (BrowseFolder(path, name))
				{
					vector<string> fileList;
					for (auto& file : directory_iterator(path))
					{
						fileList.push_back(toString(file.path()));
					}

					string envPath = "";
					string specPath = "";
					string diffPath = "";
					string brdfPath = "";

					std::regex envReg("^.*EnvHDR.dds$");
					std::regex specReg("^.*SpecularHDR.dds$");
					std::regex diffReg("^.*DiffuseHDR.dds$");
					std::regex brdfReg("^.*Brdf.dds$");
					for (const auto& file : fileList)
					{
						envPath = std::regex_match(file, envReg) ? file : envPath;
						specPath = std::regex_match(file, specReg) ? file : specPath;
						diffPath = std::regex_match(file, diffReg) ? file : diffPath;
						brdfPath = std::regex_match(file, brdfReg) ? file : brdfPath;
					}

					bool bEnvExist = !(envPath.length() == 0);
					bool bSpecExist = !(specPath.length() == 0);
					bool bDiffExist = !(diffPath.length() == 0);
					bool bBrdfExist = !(brdfPath.length() == 0);
					if (bEnvExist && bSpecExist && bDiffExist && bBrdfExist)
					{
						GetRenderer()->InitSkybox(toWString(envPath), toWString(specPath), toWString(diffPath), toWString(brdfPath));
					}
					else
					{
						wstring errorMsg = L"";
						errorMsg = bEnvExist ? errorMsg : errorMsg + L"Environment, ";
						errorMsg = bSpecExist ? errorMsg : errorMsg + L"Specular, ";
						errorMsg = bDiffExist ? errorMsg : errorMsg + L"Diffuse, ";
						errorMsg = bBrdfExist ? errorMsg : errorMsg + L"Brdf, ";
						errorMsg = errorMsg.substr(0, errorMsg.length() - 2);
						errorMsg += L" not exist.";
						MessageBox(NULL, LPCWSTR(errorMsg.c_str()), L"Error", MB_OK);
					}
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void EditorGui::DrawLeftFrame()
{
	ImGui::Begin("left option", nullptr, ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowPos(ImVec2(0, FIXED_TOP_MENUBAR_HEIGTH));
	ImGui::SetWindowSize(ImVec2(float(mLeftFrameWidth), float(mLeftFrameHeight)));

	ImGui::End();
}

void EditorGui::DrawRightFrame()
{
	ImGui::Begin("Render option", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowPos(ImVec2(float(mBottomFrameWidth), 0.f)); // Right frame overdraw menubar
	ImGui::SetWindowSize(ImVec2(float(mRightFrameWidth), float(mRightFrameHeight)));

	auto& option = GetRenderer()->GetRenderOption();

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("General"))
	{
		ImGui::Checkbox("Wireframe", &option.bWireFrame);
		static bool bMSAA;
		bMSAA = (option.AA == eAntialiasing::MSAA);
		if (ImGui::Checkbox("MSAA ON", &bMSAA))
		{
			option.AA = bMSAA ? eAntialiasing::MSAA : eAntialiasing::NONE;
			GetRenderer()->OptionDirty() = true;
		}
		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Skybox"))
	{
		int flag = 0;
		flag += ImGui::SliderFloat("IBL Strength", &option.IBLStrength, 0.0f, 1.0f);
		if (flag)
		{
			GetRenderer()->OptionDirty() = true;
		}
		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Post Processing"))
	{
		int flag = 0;
		flag += ImGui::SliderFloat("Bloom Strength", &option.BloomStrength, 0.0f, 1.0f);
		flag += ImGui::SliderFloat("Exposure", &option.Exposure, 0.0f, 10.0f);
		flag += ImGui::SliderFloat("Gamma", &option.Gamma, 0.1f, 5.0f);
		if (flag)
		{
			GetRenderer()->OptionDirty() = true;
		}
		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Material"))
	{
		int flag = 0;

		auto& mainMesh = GetRenderer()->mMeshTable[GetModelViewer()->GetModel().MeshName];
		flag += ImGui::SliderFloat("Metallic", &mainMesh->mMaterialConstData.metallicFactor, 0.0f, 1.0f);
		flag += ImGui::SliderFloat("Roughness", &mainMesh->mMaterialConstData.roughnessFactor, 0.0f, 1.0f);
		flag += ImGui::CheckboxFlags("AlbedoTexture", &mainMesh->mMaterialConstData.useAlbedoMap, 1);
		flag += ImGui::CheckboxFlags("EmissiveTexture", &mainMesh->mMaterialConstData.useEmissiveMap, 1);
		flag += ImGui::CheckboxFlags("Use NormalMapping", &mainMesh->mMaterialConstData.useNormalMap, 1);
		flag += ImGui::CheckboxFlags("Use AO", &mainMesh->mMaterialConstData.useAOMap, 1);
		flag += ImGui::CheckboxFlags("Use HeightMapping", &mainMesh->mMeshConstData.bUseHeightMap, 1);
		flag += ImGui::SliderFloat("HeightScale", &mainMesh->mMeshConstData.HeightScale, 0.0f, 0.1f);
		flag += ImGui::CheckboxFlags("Use MetallicMap", &mainMesh->mMaterialConstData.useMetallicMap, 1);
		flag += ImGui::CheckboxFlags("Use RoughnessMap", &mainMesh->mMaterialConstData.useRoughnessMap, 1);
		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Plane"))
	{
		//ImGui::Checkbox("Show Plane", &gbPlane);
		//ImGui::Checkbox("Reflection", &gPlane->bMirror);
		//ImGui::SliderFloat("Alpha", &gPlane->MirrorAlpha, 0.0f, 1.0f);
		ImGui::TreePop();
	}

	ImGui::End();
}

void EditorGui::DrawBottomFrame()
{
	ImGui::Begin("... option", nullptr, ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowPos(ImVec2(0.f, float(mLeftFrameHeight + FIXED_TOP_MENUBAR_HEIGTH)));
	ImGui::SetWindowSize(ImVec2(float(mBottomFrameWidth), float(mBottomFrameHeight)));

	ImGui::End();
}

int EditorGui::GetViewportWidth()
{
	return mScreenWidth - mLeftFrameWidth - mRightFrameWidth;
}
int EditorGui::GetViewportHeight()
{
	return mScreenHeight - FIXED_TOP_MENUBAR_HEIGTH - mBottomFrameHeight;
}
int EditorGui::GetViewportTopLeftX()
{
	return mLeftFrameWidth;
}
int EditorGui::GetViewportTopLeftY()
{
	return FIXED_TOP_MENUBAR_HEIGTH;
}
