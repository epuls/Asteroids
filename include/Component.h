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
    glm::vec3 scale{};

    void Translate();
    void SetPosition(glm::vec3 newPosition);
    void Rotate();

    inline bool GetDirty() const { return m_dirty; }
    inline void SetDirty(const bool val) { m_dirty = val; }

private:
    std::shared_ptr<Transform> m_parent;
    bool m_dirty = false;
    void Init() override{ }
    void Start() override{ }
    void Update() override{ }
    void FixedUpdate() override{ }
    void LateUpdate() override{ }
};

class MeshRenderer : public Component{
public:
    inline void SetMesh(std::shared_ptr<AssetHandle> mesh){ m_mesh = mesh; m_meshSet = true; TryRegister(); }
    inline void SetMaterial(std::shared_ptr<AssetHandle> material){ m_material = material; m_materialSet = true; TryRegister(); }

    inline Mesh& GetMesh();
    inline Material& GetMaterial();

    inline AssetHandle& GetMeshHandle() { return *m_mesh; };
    inline AssetHandle& GetMaterialHandle() { return *m_material; };
private:
    std::shared_ptr<AssetHandle> m_mesh;
    std::shared_ptr<AssetHandle> m_material;

    bool m_meshSet = false;
    bool m_materialSet = false;
    bool m_registered = false;

    uint32_t m_renderID = 0;

    Transform* m_transform;
    RenderTransform* m_renderTransform;

    void Init() override;
    void Start() override;
    void Update() override;
    void FixedUpdate() override;
    void LateUpdate() override;

    void TryRegister();

};

class ParticleSystemRenderer : public Component{

};