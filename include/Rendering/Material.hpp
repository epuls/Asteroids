#pragma once

#include <Rendering/Shader.hpp>
#include <memory>

// Stub for now
class Material{
public:
    std::shared_ptr<IShader> Shader;
};