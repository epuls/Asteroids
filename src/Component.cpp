#pragma once

#include <Component.h>
#include <Assets/AssetManager.hpp>
#include <GameObject.h>

void MeshRenderer::TryRegister() {
    if (!m_registered && m_meshSet && m_materialSet){
        m_registered = true;
        Renderer::Register(GetMaterial().type, GetMeshHandle(), GetMaterialHandle(), *m_transform);

        LOG_DEBUG("Component", "MeshRenderer registering with renderer");
    }
}

Mesh &MeshRenderer::GetMesh() {
    return m_mesh->GetManager().GetMesh(*m_mesh);
}

Material &MeshRenderer::GetMaterial(){
    return m_material->GetManager().GetMaterial(*m_material);
}

void MeshRenderer::Init() {
    m_transform = gameObject->GetComponent<Transform>();
    LOG_DEBUG("Component", "Meshrenderer referenced transform");
}

void MeshRenderer::Start() {

}

void MeshRenderer::Update() {

}

void MeshRenderer::FixedUpdate() {

}

void MeshRenderer::LateUpdate() {
    if (m_transform->GetDirty()){

    }
}

void Transform::Translate() {


    m_dirty = true;
}

void Transform::Rotate() {

}


void Transform::SetPosition(glm::vec3 newPosition) {
    position = newPosition;
    m_dirty = true;
}
