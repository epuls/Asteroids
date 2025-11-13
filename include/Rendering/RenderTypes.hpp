#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Assets/AssetHandle.h>

//Forward decl
class Transform;

enum RenderQueue{
    Opaque,
    Transparent
};


struct RenderTransform{
    glm::vec3 position{};
    glm::quat rotation{};
    glm::vec3 scale{};
};


struct RenderObject{
    bool enabled = true;
    bool isStatic = false;

    RenderQueue type;
    RenderTransform renderTransform{};
    Transform& gameObjectTransform;

    AssetHandle& mesh;
    AssetHandle& material;

    uint32_t sortKey = 0;
    uint32_t id = 0;

    void TrySyncTransforms();
    RenderObject(RenderQueue type, AssetHandle& mesh, AssetHandle& material, Transform& gameObjectTransform) : type(type), mesh(mesh), material(material), gameObjectTransform(gameObjectTransform){}
};

struct DrawRequest{
    RenderObject* renderObject;
};