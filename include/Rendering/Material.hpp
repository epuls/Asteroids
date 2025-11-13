#pragma once

#include <memory>

#include <Rendering/Shader.hpp>
#include <Rendering/RenderTypes.hpp>

#include <glad/glad.h>

//TODO: move most of IShader to here
class Material{
public:
    RenderQueue type = RenderQueue::Opaque;
    std::shared_ptr<IShader> Shader;
    std::shared_ptr<AssetHandle> tex;

    void SetTexture(std::shared_ptr<AssetHandle> textureAsset);

};