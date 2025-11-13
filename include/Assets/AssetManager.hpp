#pragma once

#include <glad/glad.h>

#include <vector>
#include <unordered_map>
#include <memory>

#include <Assets/AssetHandle.h>

#include <FileIO/AudioClip.h>
#include <Rendering/Mesh.h>
#include "Rendering/Material.hpp"


struct ApplicationContext;

template<typename T>
struct Asset{
    std::string name;
    T value;
};




class AssetManager{
private:
    ApplicationContext& m_ctx;

    // If we ever unload assets, asset handles will become invalidated
    std::vector<Asset<GLuint>> m_textures{};
    std::vector<Asset<AudioClip>> m_audioClips{};
    std::vector<Asset<Mesh>> m_meshes{};
    std::vector<Asset<Material>> m_materials{};

    std::unordered_map<std::string, AssetHandle> m_nameLookup;

    bool m_loadedTextures{};
    bool m_loadedAudioClips{};
    bool m_loadedModels{};
    bool m_loadedMaterials{};

    void LoadTextures();
    void LoadAudioClips();
    void LoadModels();
    void LoadMaterials();
    void BuildLookups();

public:
    explicit AssetManager(ApplicationContext& ctx) : m_ctx(ctx){}
    void Init();

    template<typename T>
    T& GetAsset(AssetHandle& handle, std::vector<Asset<T>>& assetStore){
        return assetStore[handle.index].value;
    }

    AssetHandle GetAssetHandle(const std::string& name) {return m_nameLookup.at(name); }
    inline GLuint& GetTexture(AssetHandle& h){ return GetAsset(h, m_textures); }
    inline AudioClip& GetAudioClip(AssetHandle& h) { return GetAsset(h, m_audioClips); }
    inline Mesh& GetMesh(AssetHandle& h) { return GetAsset(h, m_meshes); }
    inline Material& GetMaterial(AssetHandle& h) { return GetAsset(h, m_materials); }

    std::unique_ptr<AssetHandle> GetAssetHandleUPtr(const std::string& name){
        return std::make_unique<AssetHandle>(GetAssetHandle(name));
    }

    [[nodiscard]]std::shared_ptr<AssetHandle> GetAssetHandleSharedPtr(const std::string& name){
        return std::make_shared<AssetHandle>(GetAssetHandle(name));
    }


    // Resources
    std::unordered_map<std::string, std::string> textureFilenames{
            std::pair<std::string, std::string>(std::string("MainMenu"), std::string("pressspacebar.png")),
            std::pair<std::string, std::string>(std::string("ShipTex"), std::string("ship_col.png")),
            std::pair<std::string, std::string>(std::string("DestroyMedium"), std::string("destroy_medium.png")),
            std::pair<std::string, std::string>(std::string("DestroyCrescent"), std::string("destroy_crescent.png")),
            std::pair<std::string, std::string>(std::string("BGAst0"), std::string("bg_ast_2.png")),
            std::pair<std::string, std::string>(std::string("Life"), std::string("life_triangle.png")),
            std::pair<std::string, std::string>(std::string("AstTex"), std::string("ast.png"))
    };

    std::unordered_map<std::string, std::string> audioFilenames{
            std::pair<std::string, std::string>(std::string("Shoot0"), std::string("laser0.wav")),
            std::pair<std::string, std::string>(std::string("Shoot1"), std::string("laser1.wav")),
            std::pair<std::string, std::string>(std::string("Pop"), std::string("bubblepop.wav")),
            std::pair<std::string, std::string>(std::string("ImpactThud"), std::string("woodenfloorimpact.wav")),
            std::pair<std::string, std::string>(std::string("ImpactBoing"), std::string("thunderbucket.wav")),
            std::pair<std::string, std::string>(std::string("ImpactThunder"), std::string("thunderstrike.wav")),
            std::pair<std::string, std::string>(std::string("Crunch"), std::string("plasticsnatch3.wav")),
            std::pair<std::string, std::string>(std::string("Shatter0"), std::string("boxshatter2.wav")),
            std::pair<std::string, std::string>(std::string("ImpactBeat"), std::string("kbimpacts.wav")),
            std::pair<std::string, std::string>(std::string("ImpactShort"), std::string("dvimpact.wav"))
    };


    std::unordered_map<std::string, std::string> meshFilenames{
            std::pair<std::string, std::string>(std::string("ShipMesh"), std::string("ship.obj")),
            std::pair<std::string, std::string>(std::string("Asteroid"), std::string("asteroid.obj")),
    };

};