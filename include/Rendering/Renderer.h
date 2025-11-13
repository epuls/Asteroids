#pragma once

#include <vector>
#include <queue>

#include <glad/glad.h>

#include <Assets/AssetHandle.h>

#include <Rendering/Mesh.h>
#include <Rendering/Material.hpp>

#include <Utilities/Log.hpp>

#include <Rendering/RenderTypes.hpp>


// Forward declare
struct ApplicationContext;



class Renderer{
public:
    static uint32_t Register(RenderQueue type, AssetHandle& mesh, AssetHandle& material, Transform& transform){
        switch (type){
            case(RenderQueue::Opaque):
                opaques.emplace_back(type, mesh, material, transform);
                opaques.back().id = opaques.size() - 1;
                return opaques.back().id;
                LOG_DEBUG("Renderer", "Registered Opaque Render Object");
                break;
            case(RenderQueue::Transparent):
                transparents.emplace_back(type, mesh, material, transform);
                transparents.back().id = transparents.size() - 1;
                return transparents.back().id;
                LOG_DEBUG("Renderer", "Registered Transparent Render Object");
                break;
        }
    }

    static void QueueDrawRequest(DrawRequest& request){
        switch (request.renderObject->type){
            case(RenderQueue::Opaque):
                opaqueQueue.push(request);
                LOG_DEBUG("Renderer", "Queued Opaque Draw Request");
                break;
            case(RenderQueue::Transparent):
                transparentQueue.push(request);
                LOG_DEBUG("Renderer", "Queued Transparent Draw Request");
                break;
        }
    }

    void Render();

    explicit Renderer(ApplicationContext& ctx) : m_ctxt(ctx){ }

private:
    static uint32_t renderIndex;
    static std::vector<RenderObject> opaques;
    static std::queue<DrawRequest> opaqueQueue;

    static std::vector<RenderObject> transparents;
    static std::queue<DrawRequest> transparentQueue;
    ApplicationContext& m_ctxt;
};

inline uint32_t Renderer::renderIndex = 0;
inline std::vector<RenderObject> Renderer::opaques{};
inline std::queue<DrawRequest> Renderer::opaqueQueue{};

inline std::vector<RenderObject> Renderer::transparents{};
inline std::queue<DrawRequest> Renderer::transparentQueue{};


