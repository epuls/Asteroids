#include <Assets/AssetManager.hpp>
#include <ApplicationSettings.hpp>
#include <FileIO/LoadObj.hpp>

void AssetManager::Init() {
    try{
        LoadTextures();
    } catch (std::exception& e){
        std::cout << "FAILED TO LOAD TEXTURES: " << e.what() << std::endl;
    }

    try{
        LoadAudioClips();
    } catch (std::exception& e){
        std::cout << "FAILED TO LOAD AUDIO: " << e.what() << std::endl;
    }

    try{
        LoadModels();
    } catch (std::exception& e){
        std::cout << "FAILED TO LOAD MESHES: " << e.what() << std::endl;
    }

    try{
        BuildLookups();
    } catch (std::exception& e){
        std::cout << "FAILED TO BUILD ASSET LOOKUPS: " << e.what() << std::endl;
    }


}

void AssetManager::LoadTextures() {
    if (m_loadedTextures) return;
    m_loadedTextures = true;
    const std::string resourcePath = ApplicationSettings::DIRECTORY + R"(/resources/textures/)";
    m_textures.reserve(textureFilenames.size());

    for(auto& p : textureFilenames){
        m_textures.emplace_back(Asset<GLuint>(p.first, TextureFromFile(p.second, resourcePath)));
    }
}

void AssetManager::LoadAudioClips() {
    if (m_loadedAudioClips) return;
    m_loadedAudioClips = true;
    const std::string resourcePath = ApplicationSettings::DIRECTORY + R"(/resources/audio/)";
    m_audioClips.reserve(audioFilenames.size());

    for(auto& p : audioFilenames){
        auto sPath = resourcePath + std::string(p.second);
        const char* path = sPath.c_str();
        m_audioClips.emplace_back(Asset<AudioClip>(p.first, AudioClip(path)));
    }
}

void AssetManager::LoadModels() {
    if (m_loadedModels) return;
    m_loadedModels = true;
    const std::string resourcePath = ApplicationSettings::DIRECTORY + R"(/resources/models/)";
    m_meshes.reserve(meshFilenames.size());

    for(auto& p : meshFilenames){
        auto sPath = resourcePath + std::string(p.second);
        const char* path = sPath.c_str();
        std::vector<Vertex> verts;
        std::vector<Texture> textures;
        std::vector<unsigned int> indices;
        LoadOBJ(path, verts, indices);
        m_meshes.emplace_back(Asset<Mesh>(p.first, Mesh(verts, indices, textures)));
    }
}

template<typename T>
void BuildLookup(std::unordered_map<std::string, AssetHandle>& lookupTable, std::vector<Asset<T>>& assetStore, AssetHandle::AssetType type){
    for (int i = 0; i < assetStore.size(); i++){
        lookupTable.emplace(std::pair<std::string, AssetHandle>(assetStore.at(i).name, AssetHandle(type, i)));
    }
}

void AssetManager::BuildLookups() {
    BuildLookup(m_nameLookup, m_textures, AssetHandle::AssetType::TEXTURE);
    BuildLookup(m_nameLookup, m_audioClips, AssetHandle::AssetType::AUDIO);
    BuildLookup(m_nameLookup, m_meshes, AssetHandle::AssetType::MODEL);
}

