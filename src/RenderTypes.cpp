#pragma once

#include <Rendering/RenderTypes.hpp>
#include <Component.h>
#include <Game/Components/Transform.hpp>


void RenderObject::TrySyncTransforms() {
    if (!gameObjectTransform.GetDirty()) return;
    gameObjectTransform.SetDirty(false);
    renderTransform.position = gameObjectTransform.position;
    renderTransform.rotation = gameObjectTransform.rotation;
    renderTransform.scale = gameObjectTransform.scale;
    LOG_DEBUG("RenderTypes", "Synced RenderObject<->GameObject Transforms");
}