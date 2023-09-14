#include "../Gateway/SystemUI.h"
#include "../Gateway/Window.h"

#include <SDL2/SDL.h>

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

namespace Gateway
{

Ref<SystemUI> SystemUI::Create(Ref<RenderWindow> window, Ref<Context> context)
{
    return new SystemUI(window, context);
}

void SystemUI::Begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
}

void SystemUI::End()
{
    ImGui::Render();

    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
}

SystemUI::SystemUI(Ref<RenderWindow> window, Ref<Context> context)
    : WindowEventFilter(WINDOW_FILTER_ORDER_TOP, window)
{
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    SDL_Window* sdlWindow = window->GetSDLWindow();
    SDL_GLContext sdlContext = context->GetContextSDL();

    ImGui_ImplSDL2_InitForOpenGL(sdlWindow, sdlContext);
    ImGui_ImplOpenGL3_Init();
}

SystemUI::~SystemUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

bool SystemUI::OnFilter(SDL_Event* evt)
{
    if ( !m_enabled )
        return false;

    bool consumed = ImGui_ImplSDL2_ProcessEvent(evt);

    ImGuiIO& io = ImGui::GetIO();
    if ( io.WantCaptureMouse||io.WantCaptureKeyboard )
    {
        return consumed;
    }

    return false;
}


}