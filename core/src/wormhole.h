#pragma once

#include "SDL.h"

// CPU raymarched wormhole tunnel. Renders into a low-res SDL texture
// and composites a full-res overlay ring on top.
struct WormholeRenderer
{
    static const int RENDER_WIDTH = 240;
    static const int RENDER_HEIGHT = 180;

    SDL_Texture* texture = nullptr;
    uint32_t pixels[RENDER_WIDTH * RENDER_HEIGHT];

    float time = 0.0f;
    float speed = 1.0f;
    float targetSpeed = 1.0f;
    float brightness = 1.0f;
    float swirl = 1.0f;
    float collapse = 0.0f;

    SDL_Texture* overlayTex = nullptr;
    int overlayW = 0;
    int overlayH = 0;

    bool Init(SDL_Renderer* renderer, int windowW, int windowH);
    void Shutdown();
    void BuildOverlay();

    static float Hash(float n);
    static float fract(float x);
    static float smoothstep(float edge0, float edge1, float x);
    static float noise(float x, float y);
    static float fbm(float x, float y);
    static uint8_t ClampByte(float v);
    static uint32_t MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    void Update(float dt);
    void Render();
    void Present(SDL_Renderer* renderer);
    void PresentOverlay(SDL_Renderer* renderer);
};
