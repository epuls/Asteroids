#pragma once
#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <glad/glad.h>

static void fail_tmp(const char* m) { SDL_Log("%s: %s", m, SDL_GetError()); }

struct RenderText{
    GLuint id = 0;
    TTF_Font    *font;
    const float    ptSize;
    const char  *path;
    const char  *text;
    int textW;
    int textH;

    [[nodiscard]] constexpr float YScaleToPreserveAspect(float xScale) const{
        float ratio = (float)textW / (float)textH;
        return xScale / ratio;
    }


    RenderText(const char* path, const char* text, const float ptSize = 20) : path(path), text(text), ptSize(ptSize){
        auto stream = SDL_IOFromFile(path, "r");
        font = TTF_OpenFontIO(stream, false, ptSize);

        if (!font){
            fail_tmp("Failed to open font!");
        }

        SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
        SDL_Surface *s = TTF_RenderText_Blended(font, text, 0, color);
        SDL_Surface * s_rgba = SDL_ConvertSurface(s, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(s);
        if (!s_rgba) { fail_tmp("SDL_ConvertSurfaceFormat"); }


        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Ensure tight packing; ABGR8888 is 4 bytes/px
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     s_rgba->w, s_rgba->h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE,
                     s_rgba->pixels);

        textW = s_rgba->w;
        textH = s_rgba->h;

        SDL_DestroySurface(s_rgba);
    }

    ~RenderText(){
        glDeleteTextures(1, &id);
    }
};