#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <cstdio>

#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "Encryption.hpp"

typedef HRESULT( __stdcall* Present ) ( IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags );
typedef LRESULT( CALLBACK* WNDPROC )( HWND, UINT, WPARAM, LPARAM );

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#define CONSOLE_LOGGING true
 #if CONSOLE_LOGGING
  #define Log(Arg, ...) (printf("[>] " Arg "\n", __VA_ARGS__))
 #else
	#define Log()
#endif

static Present oPresent = nullptr;
static HWND window = nullptr;
static WNDPROC oWndProc = nullptr;
static ID3D11Device* pDevice = nullptr;
static ID3D11DeviceContext* pContext = nullptr;
static ID3D11RenderTargetView* mainRenderTargetView = nullptr;

namespace Globals
{
	static void* DllInstance = nullptr;
	static bool IsClosing = false;
	static bool Open = false;
	static int Tab = 0;

	namespace Aimbot
	{
		static bool Enabled = false;
		static bool DrawCrosshair = false;
		static bool DrawFOV = false;
		static bool DrawFilledFOV = false;
		static float DrawFOVSize = 150;
	}

	namespace Visuals
	{
		static bool Enabled = false;
		static bool Boxes = false;
		static bool Corners = false;
		static bool Snaplines = false;
		static bool FilledBoxes = false;
		static bool DisplayInfo = false;
		static bool DisplayHealth = false;
		static bool DisplayNames = false;
	}
}