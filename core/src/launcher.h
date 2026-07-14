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

    char serverAddr[128] = "";
    char serverPort[16] = "2106";

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
    bool Init(SDL_Renderer* renderer);
    void Shutdown();
    void HandleDragEvent(SDL_Event& event, SDL_Window* window);
    void LaunchGame();
    void Update(float dt);
    void RenderForm();
    void RenderBackground(SDL_Renderer* renderer);
    void RenderUI();
};
