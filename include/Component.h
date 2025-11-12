#pragma once

#include <memory>


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>



#include <Utilities/Log.hpp>

#include <Rendering/Material.hpp>
#include <Rendering/Mesh.h>

#include <Assets/AssetHandle.h>

// Forward declare
class GameObject;

class Component{
public:
    GameObject* gameObject;

    void BaseInit(){ Init(); }
    void BaseStart(){ Start(); }
    void BaseUpdate(){ Update(); }
    void BaseFixedUpdate(){ FixedUpdate(); }
    void BaseLateUpdate(){ LateUpdate(); }
    void BaseOnDestroy(){ OnDestroy(); }


private:
    virtual void Init(){}
    virtual void Start(){}
    virtual void Update(){}
    virtual void FixedUpdate(){}
    virtual void LateUpdate(){}
    virtual void OnDestroy(){}

};

class Transform : public Component{
public:
    glm::vec3 position{};
    glm::vec3 localPosition{};
    glm::quat rotation{};

private:
    std::shared_ptr<Transform> m_parent;
    void Init() override{ }
    void Start() override{ }
    void Update() override{ }
    void FixedUpdate() override{ }
    void LateUpdate() override{ }
};

class MeshRenderer : public Component{
public:
    AssetHandle mesh{};
    AssetHandle material{};
};

class ParticleSystemComponent : public Component{

};