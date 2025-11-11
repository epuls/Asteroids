#include <Rendering/Texture.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>


static GLuint CreateGLTexture2D(unsigned char* pixels, int w, int h, int comps) {
    GLenum fmt = (comps == 4) ? GL_RGBA : (comps == 3) ? GL_RGB : GL_RED;
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return tex;
}

GLuint TextureFromFile(std::string path, std::string directory){
    //std::cout << directory + path << "\n";
    GLuint ID = 0;
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    std::string filepath = directory + path;

    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

    if(data){
        GLenum fmt = (nrChannels == 4) ? GL_RGBA : (nrChannels == 3) ? GL_RGB : GL_RED;
        //gen tex at pointer and bind
        glGenTextures(1, &ID);

        glBindTexture(GL_TEXTURE_2D, ID);

        //set wrapping params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //set filtering params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //use loaded image
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        //std::cout << "succesfully loaded texture of dims: " << width << ", " << height << " ID: " << tex.id << "\n";

    } else {
        std::cout << "ERROR::TEXTURE::PROGRAM::FAILED_TO_LOAD_TEXTURES" << std::endl;
        std::cerr << "ABORTING DUE TO FAILED TEX LOAD";
        stbi_image_free(data);
        return ID;
    }
    stbi_image_free(data);

    return ID;
}