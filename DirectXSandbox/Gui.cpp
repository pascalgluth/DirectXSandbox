﻿#include "Gui.h"
#include <fmt/format.h>
#include "Engine.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

Graphics* gfx;

void Gui::Setup(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 25.f);
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(device, deviceContext);
    ImGui::StyleColorsLight();
    ImGui::GetStyle().ScaleAllSizes(2);

	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 15.f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	gfx = Engine::GetGFX();
}

void Gui::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Gui::Render()
{
    static bool showWindowControls = false;
    static bool showAmbientLight = false;
    static bool showPointLight = false;
    static bool showSpotLight = false;
    static bool showOverlay = true;

	static std::string chars[] = {" |", " /", "--", " \\"};
	static float time = 0;
	static int pos = 0;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::BeginMainMenuBar();
    {
	    if (ImGui::BeginMenu("Window"))
	    {
            ImGui::Checkbox("Window Controls", &showWindowControls);
            ImGui::Checkbox("Ambient Light", &showAmbientLight);
            ImGui::Checkbox("Point Light", &showPointLight);
            ImGui::Checkbox("Spot Light", &showSpotLight);
            ImGui::Checkbox("Overlay", &showOverlay);

            ImGui::EndMenu();
	    }

        if (ImGui::BeginMenu("Resolution"))
        {
            if (ImGui::MenuItem("900x600")) Engine::QueueResize(900, 600);
            if (ImGui::MenuItem("1920x1080")) Engine::QueueResize(1920, 1080);
            if (ImGui::MenuItem("2560x1440")) Engine::QueueResize(2560, 1440);
            if (ImGui::MenuItem("2560x1600")) Engine::QueueResize(2560, 1600);
            if (ImGui::MenuItem("3200x1800")) Engine::QueueResize(3200, 1800);
            if (ImGui::MenuItem("3840x2160")) Engine::QueueResize(3840, 2160);

            ImGui::EndMenu();
        }

    	time += ImGui::GetIO().DeltaTime;
    	if (time >= 0.5f)
    	{
    		time = 0;
    		if (pos + 1 > 3)
    			pos = 0;
    		else
    			pos += 1;
    	}

    	ImGui::Text(chars[pos].c_str());

        DirectX::XMFLOAT3 campos = gfx->GetCamPos();
        std::string posText = fmt::format("Position: X={:.2f} Y={:.2f} Z={:.2f}", campos.x, campos.y, campos.z);

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(posText.c_str()).x - 10.f);
        ImGui::Text(posText.c_str());

    	XMFLOAT3 camDirection = gfx->GetCamDirection();
    	std::string dirText = fmt::format("Forward: X={:.2f} Y={:.2f} Z={:.2f}", camDirection.x, camDirection.y, camDirection.z);

    	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(posText.c_str()).x - ImGui::CalcTextSize(dirText.c_str()).x - 50.f);
    	ImGui::Text(dirText.c_str());

        ImGui::EndMainMenuBar();
    }


    if (showWindowControls)
    {
	    ImGui::Begin("Window Controls");
	    {
		    ImGui::Checkbox("V-Sync", gfx->GetVSyncState());
	    }
	    ImGui::End();
    }

    if (showAmbientLight)
    	LightGui::RenderAmbientLightGui(gfx->GetAmbientLight());

	if (showPointLight)
		LightGui::RenderPointLightGui(gfx->GetPointLight());

	if (showSpotLight)
		LightGui::RenderSpotLightGui(gfx->GetSpotLight());

    if (showOverlay)
    {
	    static int corner = 1;
	    static bool hm = true;
	    ImGuiIO& io = ImGui::GetIO();
	    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	    if (corner != -1)
	    {
		    const float PAD = 10.0f;
		    const ImGuiViewport* viewport = ImGui::GetMainViewport();
		    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		    ImVec2 work_size = viewport->WorkSize;
		    ImVec2 window_pos, window_pos_pivot;
		    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
		    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
		    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		    window_flags |= ImGuiWindowFlags_NoMove;
	    }
	    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	    if (ImGui::Begin("##infoOverlay", &hm, window_flags))
	    {
		    ImGui::Text("Information");
		    ImGui::Separator();
		    if (ImGui::IsMousePosValid())
			    ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		    else
			    ImGui::Text("Mouse Position: <invalid>");

		    if (gfx->GetVSyncState())
			    ImGui::Text("FPS: %d (VSync ON)", (int)ImGui::GetIO().Framerate);
		    else
			    ImGui::Text("FPS: %d (VSync OFF)", (int)ImGui::GetIO().Framerate);

		    ImGui::Text("Frametime: %.3f ms", ImGui::GetIO().DeltaTime*1000.f);

		    ImGui::Text("Resolution: %dx%d", gfx->GetResWidth(), gfx->GetResHeight());
        
	    }
	    ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
