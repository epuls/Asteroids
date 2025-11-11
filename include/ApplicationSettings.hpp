#pragma once

#include <string>
#include <Rendering/Texture.h>
#include <unordered_map>
#include <FileIO/AudioClip.h>
#include "Rendering/Mesh.h"


inline std::unordered_map<std::string, GLuint> LoadedTextures;
inline std::unordered_map<std::string, AudioClip> LoadedAudioClips;
inline std::unordered_map<std::string, Mesh> LoadedMeshes;


struct ApplicationSettings{
    const unsigned int SCR_WIDTH = 1000;
    const unsigned int SCR_HEIGHT = 1000;
    const unsigned int GL_MJR = 4;
    const unsigned int GL_MNR = 1;
    const unsigned int GL_FULL = 410;
    const std::string TITLE = "Asteroids";
    static const std::string DIRECTORY;
};


