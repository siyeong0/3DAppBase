#include "pch.h"
#include "D3D11Gui.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <Windows.h>

D3D11Gui::D3D11Gui(HWND window, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: IDIGui()
{
	mWindow = window;
	mDevice = device;
	mContext = context;
}

D3D11Gui::~D3D11Gui()
{

}

void D3D11Gui::OnAttach()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.DisplaySize = ImVec2(float(TargetRenderer->GetRenderOption().Resolution.Width), float(TargetRenderer->GetRenderOption().Resolution.Height));

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	if (!ImGui_ImplDX11_Init(mDevice.Get(), mContext.Get()))
	{
		assert(false);
	}
	if (!ImGui_ImplWin32_Init(mWindow))
	{
		assert(false);
	}
}

void D3D11Gui::OnDetach()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void D3D11Gui::RenderBegin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}

void D3D11Gui::RenderEnd()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}