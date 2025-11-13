#pragma once
#include <Component.h>

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