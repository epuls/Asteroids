#include <GameObject.h>
#include <Game/Components/Transform.hpp>


void GameObject::Init() {
    //m_components.reserve(2);
    AddComponent<Transform>();;
    //DebugLog("GameObject", "Initialized GameObject!");
}

void GameObject::Start() {
    for(auto& c : m_allComponents){ c->BaseStart(); }
    //DebugLog("GameObject", "Called Start on GameObject!");
}

void GameObject::Update() {
    //DebugLog("GameObject", "Called Update on GameObject!");
    for(auto& c : m_allComponents){ c->BaseUpdate(); }
}

void GameObject::FixedUpdate() {
    for(auto& c : m_allComponents){ c->BaseFixedUpdate(); }
}

void GameObject::LateUpdate() {
    for(auto& c : m_allComponents){ c->BaseLateUpdate(); }
}

void GameObject::OnDestroy() {
    for(auto& c : m_allComponents){ c->BaseOnDestroy(); }
}



void GameObject::RemoveComponent(Component& c) {

}




