#pragma once
#include "dllAPI.h"
#include "IDIGui.h"

#include <d3d11.h>
#include <wrl/client.h> // ComPtr

using Microsoft::WRL::ComPtr;

class RENDERER_API D3D11Gui : public IDIGui
{
public:
	D3D11Gui(HWND window, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	virtual ~D3D11Gui();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void RenderBegin() override;
	virtual void RenderEnd() override;

protected:
	HWND mWindow;
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mContext;
};