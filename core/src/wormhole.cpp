#include "wormhole.h"

#include <cmath>
#include <cstring>

bool WormholeRenderer::Init(SDL_Renderer* renderer, int windowW, int windowH)
{
    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        RENDER_WIDTH, RENDER_HEIGHT);
    if (!texture) return false;

    overlayW = windowW;
    overlayH = windowH;
    overlayTex = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        overlayW, overlayH);
    if (!overlayTex) return false;

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(overlayTex, SDL_BLENDMODE_BLEND);

    BuildOverlay();

    return true;
}

void WormholeRenderer::Shutdown()
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (overlayTex)
    {
        SDL_DestroyTexture(overlayTex);
        overlayTex = nullptr;
    }
}

void WormholeRenderer::BuildOverlay()
{
    uint32_t* buf = new uint32_t[overlayW * overlayH];

    float cx = (float)overlayW * 0.5f;
    float cy = (float)overlayH * 0.5f;
    float maxR = (cx < cy) ? cx : cy;

    float innerR = maxR * 0.60f;
    float outerR = maxR * 0.80f;

    for (int y = 0; y < overlayH; y++)
    {
        for (int x = 0; x < overlayW; x++)
        {
            float dx = (float)x - cx;
            float dy = (float)y - cy;
            float dist = sqrtf(dx * dx + dy * dy);

            float alpha;
            if (dist < innerR)
                alpha = 0.0f;
            else if (dist > outerR)
                alpha = 1.0f;
            else
                alpha = (dist - innerR) / (outerR - innerR);

            buf[y * overlayW + x] = (uint32_t)(ClampByte(alpha * 255.0f) << 24);
        }
    }

    SDL_UpdateTexture(overlayTex, NULL, buf, overlayW * sizeof(uint32_t));
    delete[] buf;
}

float WormholeRenderer::Hash(float n)
{
    return fract(sinf(n) * 43758.5453123f);
}

float WormholeRenderer::fract(float x)
{
    return x - floorf(x);
}

float WormholeRenderer::smoothstep(float edge0, float edge1, float x)
{
    float t = (x - edge0) / (edge1 - edge0);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

float WormholeRenderer::noise(float x, float y)
{
    float ix = floorf(x);
    float iy = floorf(y);
    float fx = x - ix;
    float fy = y - iy;

    fx = fx * fx * (3.0f - 2.0f * fx);
    fy = fy * fy * (3.0f - 2.0f * fy);

    float a = Hash(ix + iy * 157.0f);
    float b = Hash(ix + 1.0f + iy * 157.0f);
    float c = Hash(ix + (iy + 1.0f) * 157.0f);
    float d = Hash(ix + 1.0f + (iy + 1.0f) * 157.0f);

    return a + (b - a) * fx + (c - a) * fy + (a - b - c + d) * fx * fy;
}

float WormholeRenderer::fbm(float x, float y)
{
    float v = 0.0f;
    float a = 0.5f;
    for (int i = 0; i < 4; i++)
    {
        v += a * noise(x, y);
        x *= 2.0f;
        y *= 2.0f;
        a *= 0.5f;
    }
    return v;
}

uint8_t WormholeRenderer::ClampByte(float v)
{
    if (v < 0.0f) return 0;
    if (v > 255.0f) return 255;
    return (uint8_t)v;
}

uint32_t WormholeRenderer::MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

void WormholeRenderer::Update(float dt)
{
    time += dt * speed;
    speed += (targetSpeed - speed) * dt * 3.0f;
}

void WormholeRenderer::Render()
{
    float t = time;
    float sw = swirl;
    float br = brightness;
    float aspect = (float)RENDER_WIDTH / (float)RENDER_HEIGHT;

    for (int y = 0; y < RENDER_HEIGHT; y++)
    {
        for (int x = 0; x < RENDER_WIDTH; x++)
        {
            float u = ((float)x / (float)RENDER_WIDTH - 0.5f) * 2.8f;
            float v = ((float)y / (float)RENDER_HEIGHT - 0.5f) * 2.8f;

            u *= aspect;

            float r = sqrtf(u * u + v * v);

            r *= (1.0f - collapse * 0.95f);

            float theta = atan2f(v, u);

            if (r < 0.001f) r = 0.001f;

            float depth = 1.0f / r;

            float tunnelZ = depth + t;

            theta += sinf(depth * 3.0f - t * 0.7f) * (sw + collapse * 5.0f);

            float texU = theta / 3.14159265f;
            float texV = tunnelZ * 0.3f;

            float n1 = fbm(texU * 4.0f + t * 0.1f, texV * 2.0f);
            float n2 = fbm(texU * 8.0f - t * 0.15f, texV * 4.0f + 3.7f);
            float n3 = noise(texU * 16.0f + t * 0.2f, texV * 8.0f);

            float pattern = n1 * 0.6f + n2 * 0.3f + n3 * 0.1f;

            float r_col = (sinf(texU * 2.0f + depth * 0.5f) * 0.3f + 0.2f) * pattern;
            float g_col = (sinf(texU * 3.0f + depth * 0.3f + 1.0f) * 0.2f + 0.4f) * pattern;
            float b_col = (cosf(texU * 1.5f + depth * 0.7f) * 0.3f + 0.7f) * pattern;

            float glow = 1.0f / (r * 3.0f + 0.1f);
            glow = glow * glow * 0.15f;
            r_col += glow * 0.3f;
            g_col += glow * 0.6f;
            b_col += glow * 1.0f;

            float streak = sinf(theta * 12.0f + depth * 5.0f - t * 2.0f) * 0.5f + 0.5f;
            streak = powf(streak, 8.0f) * 0.4f;
            r_col += streak * 0.2f;
            g_col += streak * 0.8f;
            b_col += streak * 1.0f;

            r_col *= br;
            g_col *= br;
            b_col *= br;

            pixels[y * RENDER_WIDTH + x] = MakeColor(
                ClampByte(r_col * 255.0f),
                ClampByte(g_col * 255.0f),
                ClampByte(b_col * 255.0f));
        }
    }
}

void WormholeRenderer::Present(SDL_Renderer* renderer)
{
    if (!texture) return;

    SDL_UpdateTexture(texture, NULL, pixels, RENDER_WIDTH * sizeof(uint32_t));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void WormholeRenderer::PresentOverlay(SDL_Renderer* renderer)
{
    if (!overlayTex) return;
    SDL_RenderCopy(renderer, overlayTex, NULL, NULL);
}
