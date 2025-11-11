#pragma once
#include <glad/glad.h>
#include <iostream>
#include "Texture.h"
#include <glm/glm.hpp>

// helper func
static GLuint make_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok=0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){ char log[512]; glGetShaderInfoLog(s,512,nullptr,log);}
    return s;
}

struct CameraData{
    glm::vec3 worldPos;
};

struct TransformData{
    glm::vec3 worldPos;
};



class IShader{
public:
    IShader()= default;
    virtual ~IShader() = default;

    virtual void Use() {
        glUseProgram(m_program);
        if(followCam) SetCamPos();
        else {
            glUniform2f(camPosUniformLocation, 0, 0);
        }
        //glActiveTexture(GL_TEXTURE0);
        //glUniform1i(textureSamplerLocation, 0);
    }

    virtual void Cleanup() const { glUseProgram(0); }

    bool followCam = true;

    GLuint & GetProgram() { return m_program; }

    GLuint texture0 = 0;
    GLuint textureSamplerLocation = 0;
    static float camPos[2];

    void SetCamPos() const{glUniform2f(camPosUniformLocation, camPos[0], camPos[1]);}
    GLint camPosUniformLocation = 0;

    float tint[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLuint tintLocation = 0;

    virtual void SetTextureID(GLint id){};
    virtual void SetUniformLocations(){
        camPosUniformLocation = glGetUniformLocation(GetProgram(), "camPos");
    };

private:
    GLuint m_program = 0;
    bool m_created = false;
    [[nodiscard]]virtual const char* vs() const noexcept{return "";};
    [[nodiscard]]virtual const char* fs() const noexcept{return "";};

public:
    void Create(){
        if (m_created) return;
        GLuint v = make_shader(GL_VERTEX_SHADER,   vs());
        GLuint f = make_shader(GL_FRAGMENT_SHADER, fs());
        m_program = glCreateProgram();
        glAttachShader(m_program, v);
        glAttachShader(m_program, f);
        glLinkProgram(m_program);
        glDeleteShader(v);
        glDeleteShader(f);
        m_created = true;
        glUseProgram(m_program);
        SetUniformLocations();
        glUseProgram(0);
    }
};

