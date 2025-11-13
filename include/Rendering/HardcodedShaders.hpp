#pragma once
#include <Rendering/Shader.hpp>


// Turn this into Textured Quad shader
class TexturedQuadParticle : public IShader{
public:
    void SetUniformLocations() override {
        textureSamplerLocation = glGetUniformLocation(GetProgram(), "sampleTex");
    }

    void Use() override {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glUseProgram(GetProgram());
        glProgramUniform1i(GetProgram(), textureSamplerLocation, 0);
        IShader::Use();
    }

    void Cleanup() const override {
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    ~TexturedQuadParticle(){

    }


private:
    [[nodiscard]]const char * vs() const noexcept override{
        return R"(#version 410 core
        layout(location=0) in vec3 aPos;
        layout(location=1) in vec2 aTexCoord;
        layout(location=2) in vec4 iPosSizeRot;     // (px, py, size, rotation in radians)

        out vec2 fTexCoord;
        out float fAlpha;
        out float fSize;

        uniform vec2 camPos;

        mat3 rotZ(float a){
            float c = cos(a), s = sin(a);
            return mat3(  c,-s, 0,
                          s, c, 0,
                          0, 0, 1 );
        }

        void main(){
            vec3 p = aPos;
            p = rotZ(-iPosSizeRot.w) * p;

            // scale about origin, then translate into view
            p *= iPosSizeRot.z;
            p.xy += iPosSizeRot.xy;
            //p.xy -= camPos;

            gl_Position = vec4(p, 1.0);
            fTexCoord = aTexCoord;
            fAlpha = 1.0f;
            fSize = iPosSizeRot.z;
        })";
    }

    [[nodiscard]]const char* fs() const noexcept override{
        return R"(#version 410 core
        in vec2 fTexCoord;
        in float fAlpha;
        in float fSize;

        out vec4 FragColor;

        uniform sampler2D sampleTex;

        void main(){
            //FragColor = vec4(fTexCoord, 0.0, 1.0);
            vec4 col = texture(sampleTex, fTexCoord);
            if (col.z < 0.1) discard;
            //col *= fPosSizeRot.z;
            //col *= (fSize * 0.5);
            col.x *= fSize * 10;
            col.y *= fSize * 10;
            col.z *= fSize * 10;
            FragColor = col;
            //FragColor = vec4(1,1,1,1);
        })";
    }
};

// Turn this into default particle
class DefaultRoundParticle : public IShader{
public:

    void SetUniformLocations() override {
        IShader::SetUniformLocations();
        tintLocation = glGetUniformLocation(GetProgram(), "tint");
    }

    void Use() override {
        glUseProgram(GetProgram());
        glProgramUniform4f(GetProgram(), tintLocation, tint[0], tint[1], tint[2], tint[3]);
        IShader::Use();
    }

    void Cleanup() const override {
        glUseProgram(0);
    }

private:
    [[nodiscard]]const char * vs() const noexcept override{
        return R"(#version 410 core
        layout(location=0) in vec3 aPos;
        layout(location=1) in vec2 aTexCoord;
        layout(location=2) in vec4 iPosSizeRot;     // (px, py, size, rotation in radians)
        out vec2 fTexCoord;
        out float fAlpha;

        uniform vec2 camPos;

        void main(){
            vec2 pivot = vec2(0.0, 0.0);
            vec2 scale = vec2(iPosSizeRot.z, iPosSizeRot.z);
            vec2 p = aPos.xy;
            p = (p - pivot) * scale + pivot;
            p.x += iPosSizeRot.x;
            p.y += iPosSizeRot.y;
            //p.xy -= camPos;
            gl_Position = vec4(p, aPos.z, 1.0);
            fAlpha = 1;
            fTexCoord = aTexCoord;
        })";
    }

    [[nodiscard]]const char* fs() const noexcept override{
        return R"(#version 410 core
        in vec2 fTexCoord;
        in float fAlpha;
        out vec4 FragColor;

        uniform vec4 tint;

        void main(){
            float dist = distance(fTexCoord, vec2(0.5, 0.5));
            float value = 1 - dist;
            if (value < 0.6) discard;
            value = step(0.6, value);
            vec4 col = vec4(value, value, value, value);
            col *= tint;
            FragColor = col;

            //FragColor = vec4(1.0, 1.0, 1.0, fAlpha);
        })";
    }
};


class PlayerShipShader : public IShader{
public:
    GLint translationUniformLocation;
    GLint rotationUniformLocation;
    GLint rotationXUniformLocation;
    GLint scaleUniformLocation;


    void SetTranslation(float x, float y) const{ glUniform2f(translationUniformLocation, x, y); }
    void SetRotation(float r) const{ glUniform1f(rotationUniformLocation, r); }
    void SetRotationX(float r) const{ glUniform1f(rotationXUniformLocation, r); }
    void SetScale(float x) const{ glUniform1f(scaleUniformLocation, x); }



    void Use() override {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glUseProgram(GetProgram());
        glProgramUniform1i(GetProgram(), textureSamplerLocation, 0);
        IShader::Use();
    }

    void SetUniformLocations() override {
        IShader::SetUniformLocations();
        textureSamplerLocation = glGetUniformLocation(GetProgram(), "sampleTex");
        translationUniformLocation = glGetUniformLocation(GetProgram(), "translation");
        rotationUniformLocation = glGetUniformLocation(GetProgram(), "rotation");
        rotationXUniformLocation = glGetUniformLocation(GetProgram(), "rotationX");
        scaleUniformLocation = glGetUniformLocation(GetProgram(), "scale");

        SetRotation(0);
        SetRotationX(0);
        SetTranslation(0,0);
        SetScale(1);

    }

    void Cleanup() const override {
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

private:
    [[nodiscard]]const char * vs() const noexcept override{
        return R"(#version 410 core

        layout(location=0) in vec3 aPos;
        layout(location=1) in vec3 aNormal;
        layout(location=2) in vec2 aTexCoord;

        out vec2 fTexCoord;
        out float fAlpha;

        uniform vec2 translation;
        uniform float rotation;
        uniform float rotationX;
        uniform float rotationY;
        uniform float scale;
        uniform vec2 camPos;


        const float DEG = 0;
        const float ANG = radians(DEG);
        const float SCALE = 0.1;
        const vec3  TRANSLATE = vec3(0.0, 0.0, 0.0);

        mat3 rotX(float a){
            float c = cos(a), s = sin(a);
            return mat3( 1, 0, 0,
                         0, c,-s,
                         0, s, c );
        }
        mat3 rotY(float a){
            float c = cos(a), s = sin(a);
            return mat3(  c, 0, s,
                          0, 1, 0,
                         -s, 0, c );
        }
        mat3 rotZ(float a){
            float c = cos(a), s = sin(a);
            return mat3(  c,-s, 0,
                          s, c, 0,
                          0, 0, 1 );
        }

        void main(){
            vec3 p = aPos;
            // rotate about origin, scale, then apply translation
            p = rotX(rotationX) * rotZ(-rotation) * p;
            p *= scale;
            p.xy += translation;
            p.xy -= camPos;

            gl_Position = vec4(p, 1.0);
            fTexCoord = aTexCoord;
        })";
    }

    [[nodiscard]]const char* fs() const noexcept override{
        return R"(#version 410 core
        in vec2 fTexCoord;
        in float fAlpha;
        out vec4 FragColor;

        uniform sampler2D sampleTex;

        void main(){
            vec4 col = texture(sampleTex, fTexCoord);
            FragColor = col;
        })";
    }

};




class AsteroidShader : public IShader{
public:
    GLint translationUniformLocation;
    GLint rotationUniformLocation;
    GLint scaleUniformLocation;


    void SetTranslation(float x, float y) const{ glUniform2f(translationUniformLocation, x, y); }
    void SetRotation(float r) const{ glUniform1f(rotationUniformLocation, r); }
    void SetScale(float x) const{ glUniform1f(scaleUniformLocation, x); }

    void Use() override {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glUseProgram(GetProgram());
        glProgramUniform1i(GetProgram(), textureSamplerLocation, 0);
        IShader::Use();
    }


    void SetUniformLocations() override{
        IShader::SetUniformLocations();
        translationUniformLocation = glGetUniformLocation(GetProgram(), "translation");
        rotationUniformLocation = glGetUniformLocation(GetProgram(), "rotation");
        scaleUniformLocation = glGetUniformLocation(GetProgram(), "scale");
    }

private:
    [[nodiscard]]const char * vs() const noexcept override{
        return R"(#version 410 core


        layout(location=0) in vec3 aPos;
        layout(location=1) in vec3 aNormal;
        layout(location=2) in vec2 aTexCoord;

        out vec2 fTexCoord;
        out float fAlpha;

        uniform vec2 translation;
        uniform float rotation;
        uniform float scale;
        uniform vec2 camPos;

        const float DEG = 0;
        const float ANG = radians(DEG);
        const float SCALE = 0.15;
        const vec3  TRANSLATE = vec3(0.0, 0.0, 0.0);

        mat3 rotX(float a){
            float c = cos(a), s = sin(a);
            return mat3( 1, 0, 0,
                         0, c,-s,
                         0, s, c );
        }
        mat3 rotY(float a){
            float c = cos(a), s = sin(a);
            return mat3(  c, 0, s,
                          0, 1, 0,
                         -s, 0, c );
        }
        mat3 rotZ(float a){
            float c = cos(a), s = sin(a);
            return mat3(  c,-s, 0,
                          s, c, 0,
                          0, 0, 1 );
        }

        void main(){
            vec3 p = aPos;
            p = rotZ(-rotation) * p;
            p *= scale;
            p.xy += translation;
            p.xy -= camPos;

            gl_Position = vec4(p, 1.0);
            fTexCoord = aTexCoord;
        })";
    }

    [[nodiscard]]const char* fs() const noexcept override{
        return R"(#version 410 core
        in vec2 fTexCoord;
        in float fAlpha;
        out vec4 FragColor;
        void main(){
            FragColor = vec4(fTexCoord, 0.0, 1.0);
        })";
    }

};



class UIRectShader : public IShader{
public:
    GLint translationUniformLocation;
    GLint rotationUniformLocation;
    GLint scaleUniformLocation;
    GLint opacityUniformLocation;
    GLint flipUVUniformLocation;
    GLint tileOffsetUniformLocation;

    bool flipUV = true;

    void SetTranslation(float x, float y) const{ glUniform2f(translationUniformLocation, x, y); }
    //void SetRotation(float r) const{ glUniform1f(rotationUniformLocation, r); }
    void SetScale(float x, float y) const{ glUniform2f(scaleUniformLocation, x, y); }
    void SetOpacity(float val) { glProgramUniform1f(GetProgram(), opacityUniformLocation, val); }
    void SetFlipUV() { glProgramUniform1i(GetProgram(), flipUVUniformLocation, flipUV); }
    void SetTileOffset(float x, float y) const{ glUniform2f(tileOffsetUniformLocation, x, y); }

    void SetUniformLocations() override{
        translationUniformLocation = glGetUniformLocation(GetProgram(), "translation");
        opacityUniformLocation = glGetUniformLocation(GetProgram(), "opacity");
        textureSamplerLocation = glGetUniformLocation(GetProgram(), "sampleTex");
        scaleUniformLocation = glGetUniformLocation(GetProgram(), "scale");
        flipUVUniformLocation = glGetUniformLocation(GetProgram(), "flipUV");
        tileOffsetUniformLocation = glGetUniformLocation(GetProgram(), "tileOffset");

        SetOpacity(1);
        SetTranslation(0,0);
        SetScale(1,1);
        SetFlipUV();
        SetTileOffset(0,0);

    }

private:
    [[nodiscard]]const char * vs() const noexcept override{
        return R"(#version 410 core
        layout(location=0) in vec3 aPos;
        layout(location=1) in vec2 aTexCoord;

        out vec2 fTexCoord;
        out float fAlpha;

        uniform vec2 translation;
        uniform vec2 scale;

        void main(){
            vec3 p = aPos;
            p.xy *= scale;
            p.xy += translation;

            gl_Position = vec4(p, 1.0);
            fTexCoord = aTexCoord;
            fAlpha = 1.0;
        })";
    }

    [[nodiscard]]const char* fs() const noexcept override{
        return R"(#version 410 core
        in vec2 fTexCoord;
        in float fAlpha;

        out vec4 FragColor;

        uniform sampler2D sampleTex;
        uniform float opacity;
        uniform bool flipUV;
        uniform vec2 tileOffset;


        void main(){
            vec2 flippedUVs = fTexCoord + tileOffset;
            if (flipUV){
            flippedUVs.y = 1 - flippedUVs.y;
            }
            vec4 col = texture(sampleTex, flippedUVs);
            //col.xy = flippedUVs.xy;
            //vec4 col = vec4(flippedUVs.xy, 0, 0.25);
            if (col.a < 0.05) discard;
            //col.a *= opacity;
            //col += texture(sampleTex, flippedUVs);
            FragColor = col;
        })";
    }

};

