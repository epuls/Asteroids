#pragma once

// Forward declare
class AssetManager;

class AssetHandle{
public:
    enum class AssetType{
        MODEL,
        TEXTURE,
        AUDIO,
        MATERIAL,
        PARTICLE_SYSTEM,
        UNDEFINED
    };


    AssetType assetType;
    unsigned int index;

    inline AssetManager& GetManager() { return *m_assetManager; }

    explicit AssetHandle(AssetManager* owner, AssetType type=AssetType::UNDEFINED, const unsigned int id=0) : m_assetManager(owner), assetType(type), index(id){}


private:
    AssetManager* m_assetManager;
};


