#include <Rendering/Renderer.h>
#include <ApplicationContext.hpp>



void Renderer::Render() {
    for(auto& ro : opaques){
        if (!ro.enabled) continue;
        ro.TrySyncTransforms();
        m_ctxt.assetManager->GetMaterial(ro.material).Shader->Use();
        m_ctxt.assetManager->GetMesh(ro.mesh).draw();
    }

    for(auto& ro : transparents){
        if (!ro.enabled) continue;
        ro.TrySyncTransforms();
        m_ctxt.assetManager->GetMaterial(ro.material).Shader->Use();
        m_ctxt.assetManager->GetMesh(ro.mesh).draw();
    }
}