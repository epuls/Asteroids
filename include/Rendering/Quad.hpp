#pragma once

#include <glad/glad.h>
#include <Rendering/Shader.hpp>
#include <memory>
#include <vector>
#include <span>

#include <cstring>
#include <iostream>


struct Quad{
    static constexpr float vertices[] = {
            /*vert BL */-0.5f, -0.5f, 0.0f,/*uv coords*/ 0.0f, 0.0f,
            /*vert TL */-0.5f, 0.5f, 0.0f,/*uv coords*/ 0.0f, 1.0f,
            /*vert TR */0.5f, 0.5f, 0.0,/*uv coords*/ 1.0f, 1.0f,
            /*vert BR */0.5f, -0.5f, 0.0,/*uv coords*/ 1.0f, 0.0f
    };

    static constexpr unsigned int indices[] = {
            0, 3, 1,//BL->BR->TL
            1, 3, 2//TL->BR->TR
    };
};

struct UIQuad{
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLuint program = 0;

    inline void InitBuffers(){
        // Init Buffers

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &ebo);
        glGenBuffers(1, &vbo);

        // Create Vertex Array
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Quad::vertices), Quad::vertices, GL_STATIC_DRAW);

        // Create Element Array
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Quad::indices), Quad::indices, GL_STATIC_DRAW);

        // Create vertex attrrib pointers
        // stride = vertex (3 floats) + uv (2 floats) = 5 floats
        constexpr GLsizei stride = 5 * sizeof(float);

        // location 0 = pos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

        // location 1 = uv
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

        // clean state
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    inline void draw(){
        //glUseProgram(program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(Quad::indices), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    inline void cleanup(){
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        if (vbo)            { glDeleteBuffers(1, &vbo);            vbo = 0; }
        if (ebo)            { glDeleteBuffers(1, &ebo);            ebo = 0; }
        if (vao)            { glDeleteVertexArrays(1, &vao);        vao = 0; }
    }


};
