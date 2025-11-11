#pragma once
#include <vector>
#include <glad/glad.h>
#include <Rendering/Vertex.h>

#include <iostream>

//stub for now
struct Texture{};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures; // material-bound textures for this mesh
    bool uploaded{false};

    // GPU handles (created lazily on first upload)
    GLuint vao{0}, vbo{0}, ebo{0};

    Mesh() = default;
    Mesh(std::vector<Vertex> v, std::vector<unsigned> i, std::vector<Texture> t)
            : vertices(std::move(v)), indices(std::move(i)), textures(std::move(t)) {
        //std::cout << "creating mesh\n";
    }


    void upload();
    void destroy();

    ~Mesh();

    // Utility: simple draw assuming currently bound shader reads standard locations
    void draw() const;
};