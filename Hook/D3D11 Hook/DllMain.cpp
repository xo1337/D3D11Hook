#include "Globals.hpp"

void OnDraw( );
void InitImGui()
{
	ImGui::CreateContext();

	auto io = ImGui::GetIO( );
	auto& style = ImGui::GetStyle( );

	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	style.WindowMinSize = ImVec2( 128, 128 );
	style.WindowTitleAlign = ImVec2( 0.5, 0.5 );
	style.FrameBorderSize = 1;
	style.ChildBorderSize = 1;
	style.WindowBorderSize = 1;
	style.WindowRounding = 0;
	style.FrameRounding = 0;
	style.ChildRounding = 0;
	style.Colors[ ImGuiCol_TitleBg ] = ImColor( 70, 70, 70 );
	style.Colors[ ImGuiCol_TitleBgActive ] = ImColor( 70, 70, 70 );
	style.Colors[ ImGuiCol_TitleBgCollapsed ] = ImColor( 70, 70, 70 );
	style.Colors[ ImGuiCol_WindowBg ] = ImColor( 25, 25, 25, 240 );
	style.Colors[ ImGuiCol_CheckMark ] = ImColor( 70, 70, 70 );
	style.Colors[ ImGuiCol_Border ] = ImColor( 70, 70, 70 );
	style.Colors[ ImGuiCol_Button ] = ImColor( 32, 32, 32 );
	style.Colors[ ImGuiCol_ButtonActive ] = ImColor( 42, 42, 42 );
	style.Colors[ ImGuiCol_ButtonHovered ] = ImColor( 42, 42, 42 );
	style.Colors[ ImGuiCol_ChildBg ] = ImColor( 45, 45, 45 );
	style.Colors[ ImGuiCol_FrameBg ] = ImColor( 32, 32, 32 );
	style.Colors[ ImGuiCol_FrameBgActive ] = ImColor( 42, 42, 42 );
	style.Colors[ ImGuiCol_FrameBgHovered ] = ImColor( 42, 42, 42 );
	style.Colors[ ImGuiCol_SliderGrab ] = ImColor( 255, 255, 255 );
	style.Colors[ ImGuiCol_SliderGrabActive ] = ImColor( 255, 255, 255 );

	io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Tahoma.ttf", 14.0f );

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

__forceinline LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( uMsg == WM_KEYUP && wParam == VK_HOME )
		Globals::Open ^= 1;

	if ( Globals::Open )
	{
		ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam );
		return true;
	}

	return CallWindowProcA(oWndProc, hWnd, uMsg, wParam, lParam);
}

__forceinline HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	static bool SetupHook = false;

	if (!SetupHook)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);

			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;

			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();

			oWndProc = (WNDPROC)SetWindowLongA(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
		
			SetupHook = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::GetIO( ).MouseDrawCursor = Globals::Open;
	if ( Globals::Open )
	{
		OnDraw( );
	}

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

void OnDraw()
{
	static auto Slider = [ & ] ( const char* label, float* value, float min, float max, float width = ImGui::GetContentRegionAvail( ).x )
	{
		ImGui::PushID( label );
		ImGui::PushItemWidth( width );
		ImGui::SliderFloat( _( "##CustomSliderF_" ), value, min, max );
		ImGui::PopItemWidth( );
		ImGui::PopID( );
		ImGui::Spacing( );
	};

	static auto Checkbox = [ & ] ( const char* label, bool* value )
	{
		ImGui::PushID( label );
		ImGui::Checkbox( label, value );
		ImGui::PopID( );
		ImGui::Spacing( );
	};

	ImGui::Begin( _("D3D11 Hook"), 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );
	{
		if ( ImGui::Button( _( "Aimbot" ), ImVec2( ImGui::GetContentRegionAvail( ).x / 4, 29 ) ) )
			Globals::Tab = 0;

		ImGui::SameLine( );

		if ( ImGui::Button( _( "Visuals" ), ImVec2( ImGui::GetContentRegionAvail( ).x / 3, 29 ) ) )
			Globals::Tab = 1;

		ImGui::SameLine( );

		if ( ImGui::Button( _( "Other" ), ImVec2( ImGui::GetContentRegionAvail( ).x / 2, 29 ) ) )
			Globals::Tab = 2;
		
		ImGui::SameLine( );

		if ( ImGui::Button( _( "Settings" ), ImVec2( ImGui::GetContentRegionAvail( ).x / 1, 29 ) ) )
			Globals::Tab = 3;

		ImGui::Spacing( );
		ImGui::Separator( );

		switch ( Globals::Tab )
		{
		case 0: // Aimbot
			Checkbox( _( "Aimbot Enabled"), &Globals::Aimbot::Enabled );
			Checkbox( _( "Draw FOV"), &Globals::Aimbot::DrawFOV );
			Checkbox( _( "Draw Filled FOV"), &Globals::Aimbot::DrawFilledFOV );
			Checkbox( _( "Draw Crosshair"), &Globals::Aimbot::DrawCrosshair );
			Slider( _( "FOV Size"), &Globals::Aimbot::DrawFOVSize, 0, 650 );
			break;

		case 1: // Visuals
			Checkbox( _( "ESP Enabled"), &Globals::Visuals::Enabled );
			Checkbox( _( "Boxes"), &Globals::Visuals::Boxes );
			Checkbox( _( "Filled Boxes"), &Globals::Visuals::FilledBoxes );
			Checkbox( _( "Snaplines"), &Globals::Visuals::Snaplines );
			Checkbox( _( "Display Info"), &Globals::Visuals::DisplayInfo );
			Checkbox( _("Display Health"), &Globals::Visuals::DisplayHealth );
			Checkbox( _( "Display Names" ), &Globals::Visuals::DisplayNames );
			break;

		case 2: // Other
			break;

		case 3: // Settings
			if ( ImGui::Button( _( "Unhook" ) ) )
				Globals::IsClosing = true;
			break;
		}	
	}
	ImGui::End( );

	// Cheat loop here lol

	{
		const auto draw = ImGui::GetBackgroundDrawList( );
		static const auto size = ImGui::GetIO( ).DisplaySize;
		static const auto center = ImVec2( size.x / 2, size.y / 2 );

		if ( Globals::Aimbot::DrawFOV )
			draw->AddCircle( center, Globals::Aimbot::DrawFOVSize, ImColor( 255, 255, 255 ), 100 );
		
		if ( Globals::Aimbot::DrawFilledFOV )
			draw->AddCircleFilled( center, Globals::Aimbot::DrawFOVSize, ImColor( 0, 0, 0, 140 ), 100 );

		if ( Globals::Aimbot::DrawCrosshair )
		{
			static const auto whiteColor = ImColor( 255, 255, 255 );
			static const auto mainColor = ImColor( 255, 0, 0 );

			draw->AddLine( center, ImVec2( center.x + 4, center.y + 4 ), whiteColor, 0.7f );
			draw->AddLine( center, ImVec2( center.x + 4, center.y - 4 ), whiteColor, 0.7f );
			draw->AddLine( center, ImVec2( center.x - 4, center.y - 4 ), whiteColor, 0.7f );
			draw->AddLine( center, ImVec2( center.x - 4, center.y + 4 ), whiteColor, 0.7f );
			draw->AddLine( ImVec2( center.x + 4, center.y + 4 ), ImVec2( center.x + 4 + 4, center.y + 4 + 4 ), mainColor, 0.7f );
			draw->AddLine( ImVec2( center.x + 4, center.y - 4 ), ImVec2( center.x + 4 + 4, center.y - 4 - 4 ), mainColor, 0.7f );
			draw->AddLine( ImVec2( center.x - 4, center.y - 4 ), ImVec2( center.x - 4 - 4, center.y - 4 - 4 ), mainColor, 0.7f );
			draw->AddLine( ImVec2( center.x - 4, center.y + 4 ), ImVec2( center.x - 4 - 4, center.y + 4 + 4 ), mainColor, 0.7f );
		}

		if ( Globals::Visuals::Enabled )
		{
			if ( Globals::Visuals::Boxes ) {

			}

			if ( Globals::Visuals::Corners ) {

			}

			if ( Globals::Visuals::FilledBoxes ) {

			}

			if ( Globals::Visuals::Snaplines ) {
				
			}

			if ( Globals::Visuals::DisplayInfo ) {

			}

			if ( Globals::Visuals::DisplayHealth ) {

			}

			if ( Globals::Visuals::DisplayNames ) {

			}
		}
	}
}

int MainThread()
{
#if CONSOLE_LOGGING
	AllocConsole( );
	freopen_s( ( FILE** )stdout, "CONOUT$", "w", stdout );
#endif

	Log( "Injected at: %s", __TIME__ );
	Log( "Module Instance: 0x%p", reinterpret_cast<void*>(Globals::DllInstance) );

	static bool Done = false;

	while ( !Done )
	{
		if ( kiero::init( kiero::RenderType::D3D11 ) == kiero::Status::Success )
		{
			kiero::bind( 8, ( void** )&oPresent, hkPresent );
			Done = true;
		}
	}

	while ( true )
	{
		if ( ( GetAsyncKeyState( VK_END ) & 1 ) || Globals::IsClosing )
			break;

		Sleep( 1000 );
	}

	Log( "Unhooked at: %s", __TIME__ );

	Globals::IsClosing = false;
	Globals::Open = false;

	kiero::unbind( 8 );
	kiero::shutdown( );

	FreeLibrary ( (HMODULE)Globals::DllInstance );
	return TRUE;
}

int __stdcall DllMain(HMODULE hModule, DWORD dwReason, void* )
{
	Globals::DllInstance = hModule;

	if ( dwReason == DLL_PROCESS_ATTACH )
		CreateThread( 0, 0, reinterpret_cast< PTHREAD_START_ROUTINE >( MainThread ), 0, 0, 0 );

	return TRUE;
}