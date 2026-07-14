#pragma once

#include "imgui.h"
#include "wormhole.h"

enum LauncherState
{
    STATE_FORM,
    STATE_WORMHOLE,
    STATE_EXITING,
    STATE_LAUNCH,
    STATE_EXIT
};

// Main application. Manages the ImGui connection form, window dragging,
// game version detection and the wormhole transition on launch/exit.
struct Launcher
{
    LauncherState state = STATE_FORM;
    WormholeRenderer wormhole;

    char serverUrl[1024] = "";

    float transitionTimer = 0.0f;
    static const float TRANSITION_DURATION;
    static const float FLASH_TIME;

    float exitTimer = 0.0f;
    static const float EXIT_DURATION;

    float formAlpha = 1.0f;

    bool dragging = false;
    int dragStartX = 0;
    int dragStartY = 0;
    int windowStartX = 0;
    int windowStartY = 0;

    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 600;

    char detectedVersion[64] = "";
    bool focusedAddr = false;

    void DetectVersion();
    bool CheckGameExecutable();
    void ParseConnectionString(char* user, size_t userSz, char* pass, size_t passSz,
                               char* addr, size_t addrSz, char* port, size_t portSz);
    bool Init(SDL_Renderer* renderer);
    void Shutdown();
    void HandleDragEvent(SDL_Event& event, SDL_Window* window);
    void LaunchGame();
    void Update(float dt);
    void RenderForm();
    void RenderBackground(SDL_Renderer* renderer);
    void RenderUI();
};
