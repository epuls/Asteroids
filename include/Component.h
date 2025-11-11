#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>

#include <Utilities/Log.hpp>

// Forward declare
class GameObject;

class Component{
public:
    explicit Component(GameObject& go) : gameObject(go){}

    GameObject& gameObject;

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
    explicit Transform(GameObject& go) : Component(go){}
    glm::vec3 position{};
    glm::vec3 localPosition{};
    glm::quat rotation{};

private:
    std::shared_ptr<Transform> m_parent;
    void Init() override{ DebugLog("Component", "Initialized Transform!"); }
    void Start() override{ DebugLog("Component", "Called Start on Transform!"); }
    void Update() override{DebugLog("Component", "Called Update on Transform!"); }
    void FixedUpdate() override{DebugLog("Component", "Called FixedUpdate on Transform!"); }
    void LateUpdate() override{DebugLog("Component", "Called LateUpdate on Transform!"); }
};

class MeshComponent : public Component{

};

class ParticleSystemComponent : public Component{

};