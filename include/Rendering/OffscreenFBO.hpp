#pragma once
#include <SDL3/SDL.h>
#include <glad/glad.h>


struct OffscreenFBO {
    GLuint fbo = 0;
    GLuint color = 0;
    GLuint depth = 0;
    int w = 0, h = 0;
};

inline OffscreenFBO CreateOffscreen(int w, int h) {
    OffscreenFBO o; o.w = w; o.h = h;

    glGenFramebuffers(1, &o.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, o.fbo);

    // Color texture
    glGenTextures(1, &o.color);
    glBindTexture(GL_TEXTURE_2D, o.color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // helpful for tiling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, o.color, 0);

    // Depth
    glGenRenderbuffers(1, &o.depth);
    glBindRenderbuffer(GL_RENDERBUFFER, o.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, o.depth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        SDL_Log("FBO incomplete: 0x%X", status);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return o;
}
