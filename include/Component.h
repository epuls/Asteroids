#pragma once

#include <memory>


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Utilities/Log.hpp>

#include <Rendering/Material.hpp>
#include <Rendering/Mesh.h>
#include <Rendering/Renderer.h>

#include <Assets/AssetHandle.h>

#include <Assets/AssetManager.hpp>


// Forward declare
class GameObject;

class Component{
public:
    GameObject* gameObject = nullptr;

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

