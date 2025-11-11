#pragma once

struct AssetHandle{
    enum class AssetType{
        MODEL,
        TEXTURE,
        AUDIO,
        UNDF
    };

    AssetType assetType;
    unsigned int index;

    AssetHandle(AssetType type=AssetType::UNDF, const unsigned int id=0) : assetType(type), index(id){}
};