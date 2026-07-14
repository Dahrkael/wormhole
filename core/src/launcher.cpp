#include "launcher.h"

#include "imgui.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
#endif

#include <cstdio>
#include <cstring>

const float Launcher::TRANSITION_DURATION = 3.0f;
const float Launcher::FLASH_TIME = 2.55f;
const float Launcher::EXIT_DURATION = 1.5f;

void Launcher::DetectVersion()
{
    FILE* f = fopen("version.ini", "r");
    if (!f) return;

    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        char* p = strchr(line, '=');
        if (p)
        {
            while (*(++p) == ' ');
            char* end = p + strlen(p) - 1;
            while (end > p && (*end == '\n' || *end == '\r')) *end-- = '\0';

            if (strstr(line, "Version") || strstr(line, "version"))
            {
                snprintf(detectedVersion, sizeof(detectedVersion), "%s", p);
                break;
            }
        }
    }
    fclose(f);
}

bool Launcher::CheckGameExecutable()
{
    FILE* f = fopen("tabula_rasa.exe", "rb");
    if (f)
    {
        fclose(f);
        return true;
    }
    return false;
}

void Launcher::ParseConnectionString(char* user, size_t userSz, char* pass, size_t passSz,
                                     char* addr, size_t addrSz, char* port, size_t portSz)
{
    user[0] = pass[0] = addr[0] = '\0';
    snprintf(port, portSz, "2106");

    const char* at = strchr(serverUrl, '@');

    if (at)
    {
        const char* colon = (const char*)memchr(serverUrl, ':', at - serverUrl);
        if (colon)
        {
            size_t ulen = colon - serverUrl;
            size_t plen = at - colon - 1;
            if (ulen >= userSz) ulen = userSz - 1;
            if (plen >= passSz) plen = passSz - 1;
            memcpy(user, serverUrl, ulen); user[ulen] = '\0';
            memcpy(pass, colon + 1, plen); pass[plen] = '\0';
        }
        else
        {
            size_t ulen = at - serverUrl;
            if (ulen >= userSz) ulen = userSz - 1;
            memcpy(user, serverUrl, ulen); user[ulen] = '\0';
        }
        at++;
    }
    else
    {
        at = serverUrl;
    }

    const char* colon = strchr(at, ':');
    if (colon)
    {
        size_t alen = colon - at;
        if (alen >= addrSz) alen = addrSz - 1;
        memcpy(addr, at, alen); addr[alen] = '\0';
        snprintf(port, portSz, "%s", colon + 1);
    }
    else
    {
        snprintf(addr, addrSz, "%s", at);
    }
}

bool Launcher::Init(SDL_Renderer* renderer)
{
    DetectVersion();
    return wormhole.Init(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Launcher::Shutdown()
{
    wormhole.Shutdown();
}

void Launcher::HandleDragEvent(SDL_Event& event, SDL_Window* window)
{
    ImGuiIO& io = ImGui::GetIO();

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        if (!io.WantCaptureMouse)
        {
            dragging = true;
            int mx, my;
            SDL_GetGlobalMouseState(&mx, &my);
            dragStartX = mx;
            dragStartY = my;
            SDL_GetWindowPosition(window, &windowStartX, &windowStartY);
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        dragging = false;
    }
    else if (event.type == SDL_MOUSEMOTION && dragging)
    {
        int mx, my;
        SDL_GetGlobalMouseState(&mx, &my);
        SDL_SetWindowPosition(window,
            windowStartX + (mx - dragStartX),
            windowStartY + (my - dragStartY));
    }
}

void Launcher::LaunchGame()
{
    char user[17] = "", pass[17] = "", addr[128] = "", port[16] = "";
    ParseConnectionString(user, sizeof(user), pass, sizeof(pass),
                          addr, sizeof(addr), port, sizeof(port));

#ifdef _WIN32
    char cmdline[512];
    snprintf(cmdline, sizeof(cmdline),
        "tabula_rasa.exe /NoPatch /AuthServer=%s:%s", addr, port);

    if (user[0] != '\0')
        snprintf(cmdline + strlen(cmdline), sizeof(cmdline) - strlen(cmdline),
            " /user=%s", user);
    if (pass[0] != '\0')
        snprintf(cmdline + strlen(cmdline), sizeof(cmdline) - strlen(cmdline),
            " /password=%s", pass);

    STARTUPINFOA si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};

    CreateProcessA(NULL, cmdline, NULL, NULL, FALSE, 0,
        NULL, NULL, &si, &pi);

    if (pi.hProcess) CloseHandle(pi.hProcess);
    if (pi.hThread) CloseHandle(pi.hThread);
#else
    char authServer[160];
    snprintf(authServer, sizeof(authServer), "%s:%s", addr, port);

    pid_t pid = fork();
    if (pid == 0)
    {
        if (user[0] != '\0' && pass[0] != '\0')
            execlp("./tabula_rasa.exe", "tabula_rasa.exe",
                "/NoPatch", "/AuthServer", authServer,
                "/user", user, "/password", pass,
                (char*)NULL);
        else if (user[0] != '\0')
            execlp("./tabula_rasa.exe", "tabula_rasa.exe",
                "/NoPatch", "/AuthServer", authServer,
                "/user", user,
                (char*)NULL);
        else
            execlp("./tabula_rasa.exe", "tabula_rasa.exe",
                "/NoPatch", "/AuthServer", authServer,
                (char*)NULL);
        _exit(1);
    }
    (void)pid;
#endif
}

void Launcher::Update(float dt)
{
    switch (state)
    {
    case STATE_FORM:
        wormhole.targetSpeed = 0.3f;
        wormhole.Update(dt);
        break;

    case STATE_WORMHOLE:
    {
        transitionTimer += dt;

        float progress = transitionTimer / TRANSITION_DURATION;
        if (progress > 1.0f) progress = 1.0f;

        wormhole.targetSpeed = 1.0f + progress * 25.0f;
        wormhole.swirl = 1.0f + progress * 2.0f;

        if (progress < 0.3f)
            formAlpha = 1.0f - (progress / 0.3f);
        else
            formAlpha = 0.0f;

        if (progress > 0.85f)
            wormhole.brightness = 1.0f + (progress - 0.85f) / 0.15f * 10.0f;

        wormhole.Update(dt);

        if (transitionTimer >= TRANSITION_DURATION)
            state = STATE_LAUNCH;
        break;
    }

    case STATE_EXITING:
    {
        exitTimer += dt;
        float progress = exitTimer / EXIT_DURATION;
        if (progress > 1.0f) progress = 1.0f;

        if (progress < 0.2f)
            formAlpha = 1.0f - (progress / 0.2f);
        else
            formAlpha = 0.0f;

        if (progress > 0.15f)
        {
            float collapseProgress = (progress - 0.15f) / 0.65f;
            if (collapseProgress > 1.0f) collapseProgress = 1.0f;
            wormhole.collapse = collapseProgress;
        }

        if (progress < 0.5f)
            wormhole.targetSpeed = 1.0f + progress * 20.0f;
        else
            wormhole.targetSpeed = 11.0f * (1.0f - (progress - 0.5f) / 0.5f);

        if (progress > 0.3f && progress < 0.6f)
            wormhole.brightness = 1.0f + (progress - 0.3f) / 0.3f * 5.0f;
        else if (progress >= 0.6f)
            wormhole.brightness = 6.0f * (1.0f - (progress - 0.6f) / 0.4f);

        wormhole.Update(dt);

        if (exitTimer >= EXIT_DURATION)
            state = STATE_EXIT;
        break;
    }

    case STATE_LAUNCH:
        LaunchGame();
        state = STATE_EXIT;
        break;

    case STATE_EXIT:
        break;
    }
}

void Launcher::RenderForm()
{
    if (formAlpha <= 0.0f) return;

    ImGuiIO& io = ImGui::GetIO();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.FramePadding = ImVec2(8, 5);
    style.ItemSpacing = ImVec2(8, 6);
    style.WindowPadding = ImVec2(10, 10);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.05f, 0.10f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.0f, 0.7f, 0.9f, 0.7f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.10f, 0.18f, 0.9f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1f, 0.15f, 0.25f, 0.9f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.18f, 0.3f, 0.9f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.07f, 0.12f, 0.9f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.12f, 0.2f, 0.9f);
    colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.1f, 0.15f, 0.25f, 0.3f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.2f, 0.3f, 0.4f);
    colors[ImGuiCol_Text] = ImVec4(0.0f, 0.85f, 1.0f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.0f, 0.4f, 0.5f, 0.8f);
    colors[ImGuiCol_Header] = ImVec4(0.0f, 0.35f, 0.55f, 0.8f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.5f, 0.7f, 0.9f);
    colors[ImGuiCol_Separator] = ImVec4(0.0f, 0.5f, 0.7f, 0.4f);

    ImVec2 windowSize(300, 230);
    ImVec2 windowPos(
        (io.DisplaySize.x - windowSize.x) * 0.5f,
        (io.DisplaySize.y - windowSize.y) * 0.5f
    );

    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowBgAlpha(formAlpha * 0.92f);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("##form", NULL, flags);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.9f, 1.0f, formAlpha));
    const char* title = "WORMHOLE LAUNCHER";
    ImVec2 titleSize = ImGui::CalcTextSize(title);
    ImGui::SetCursorPosX((windowSize.x - titleSize.x) * 0.5f);
    ImGui::Text("%s", title);
    ImGui::PopStyleColor();

    if (detectedVersion[0])
    {
        char versionBuf[128];
        snprintf(versionBuf, sizeof(versionBuf), "Detected version %s", detectedVersion);
        ImVec2 verSize = ImGui::CalcTextSize(versionBuf);
        ImGui::SetCursorPosX((windowSize.x - verSize.x) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.5f, 0.6f, formAlpha));
        ImGui::Text("%s", versionBuf);
        ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const char* destLabel = "Server";
    ImVec2 destSize = ImGui::CalcTextSize(destLabel);
    ImGui::SetCursorPosX((windowSize.x - destSize.x) * 0.5f);
    ImGui::Text("%s", destLabel);

    ImGui::Spacing();

    ImGui::PushItemWidth(-1);
    if (!focusedAddr)
    {
        ImGui::SetKeyboardFocusHere();
        focusedAddr = true;
    }
    ImGui::InputTextWithHint("##url", "user:pass@address:port", serverUrl, sizeof(serverUrl));
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Spacing();

    bool canLaunch = serverUrl[0] != '\0';
    if (canLaunch)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.3f, 1.0f));
    else
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.4f, 0.15f, 0.4f));

    if (ImGui::Button("LAUNCH", ImVec2(-1, 28)) && canLaunch)
    {
        state = STATE_WORMHOLE;
        transitionTimer = 0.0f;
        formAlpha = 1.0f;
    }
    ImGui::PopStyleColor();

    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button("EXIT", ImVec2(-1, 26)))
    {
        state = STATE_EXITING;
        exitTimer = 0.0f;
    }
    ImGui::PopStyleColor();

    ImGui::End();
}

void Launcher::RenderBackground(SDL_Renderer* renderer)
{
    wormhole.Render();
    wormhole.Present(renderer);
    wormhole.PresentOverlay(renderer);
}

void Launcher::RenderUI()
{
    if (state == STATE_FORM || (state == STATE_WORMHOLE && formAlpha > 0.0f)
        || (state == STATE_EXITING && formAlpha > 0.0f))
    {
        RenderForm();
    }
}
