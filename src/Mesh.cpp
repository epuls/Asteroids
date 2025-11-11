#include <Rendering/Mesh.h>
#include <iostream>


Mesh::~Mesh(){
    destroy();
}

void Mesh::upload() {
    if (uploaded) { return; }
    uploaded = true;
    vao = 0;
    vbo = 0;
    ebo = 0;


    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);


    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


    // layout: 0=pos,1=normal,2=uv
    constexpr GLsizei STRIDE = sizeof(Vertex);
    std::size_t off = 0;

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE, (void*)off);
    off += sizeof(float[3]);


    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE, (void*)off);
    off += sizeof(float[3]);


    // UV
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE, (void*)off);


    // tidy state
    glBindVertexArray(0);


}

void Mesh::destroy() {
    if (ebo) { glDeleteBuffers(1, &ebo); ebo = 0; }
    if (vbo) { glDeleteBuffers(1, &vbo); vbo = 0; }
    if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
}


void Mesh::draw() const {
    if (!vao)
    {
        std::cerr << "Trying to draw mesh with no bound VAO!" << std::endl;
        return;
    }

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
    glBindVertexArray(0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}