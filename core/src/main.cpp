#include <cmath>
#include <cstdio>

#include "SDL.h"
#include "SDL_syswm.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "launcher.h"

#ifdef _WIN32
#include <windows.h>

static void MakeWindowTransparent(SDL_Window* window)
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi))
        return;

    HWND hwnd = wmi.info.win.window;
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_LAYERED;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    // Color key: pure black becomes transparent
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
}
#endif

int main(int, char**)
{
    // Check if game executable exists before anything else
    FILE* check = fopen("tabula_rasa.exe", "rb");
    if (!check)
    {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Wormhole Launcher",
            "tabula_rasa.exe not found.\n"
            "Make sure the launcher is in the game client folder.",
            NULL);
        SDL_Quit();
        return 1;
    }
    fclose(check);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // Create window (borderless, fixed size, centered)
    SDL_Window* window = SDL_CreateWindow(
        "Wormhole - Tabula Rasa",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        Launcher::WINDOW_WIDTH, Launcher::WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer with vsync
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

#ifdef _WIN32
    MakeWindowTransparent(window);
#endif

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Setup ImGui backends for SDL2 + SDL_Renderer
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    // Initialize launcher
    Launcher launcher;
    if (!launcher.Init(renderer))
    {
        SDL_Log("Failed to initialize wormhole renderer");
        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Frame timing
    Uint64 lastTime = SDL_GetPerformanceCounter();
    Uint64 freq = SDL_GetPerformanceFrequency();

    // Main loop
    bool done = false;
    while (!done)
    {
        // Calculate delta time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float dt = (float)(currentTime - lastTime) / (float)freq;
        lastTime = currentTime;

        // Clamp dt to avoid spiral of death
        if (dt > 0.1f) dt = 0.1f;

        // Poll events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            launcher.HandleDragEvent(event, window);

            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                done = true;
        }

        // Update launcher state
        launcher.Update(dt);

        if (launcher.state == STATE_EXIT)
            done = true;

        // Start ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Build ImGui form
        launcher.RenderUI();

        // Finalize ImGui
        ImGui::Render();

        // Clear and render layers
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Layer 1: Wormhole background
        launcher.RenderBackground(renderer);

        // Layer 2: ImGui overlay
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    launcher.Shutdown();
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
