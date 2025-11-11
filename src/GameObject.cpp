#include <GameObject.h>


void GameObject::Init() {
    AddComponent<Transform>();
    //DebugLog("GameObject", "Initialized GameObject!");
}

void GameObject::Start() {
    for(auto& c : m_components){ c->BaseStart(); }
    //DebugLog("GameObject", "Called Start on GameObject!");
}

void GameObject::Update() {
    //DebugLog("GameObject", "Called Update on GameObject!");
    for(auto& c : m_components){ c->BaseUpdate(); }
}

void GameObject::FixedUpdate() {
    for(auto& c : m_components){ c->BaseFixedUpdate(); }
}

void GameObject::LateUpdate() {
    for(auto& c : m_components){ c->BaseLateUpdate(); }
}

void GameObject::OnDestroy() {
    for(auto& c : m_components){ c->BaseOnDestroy(); }
}

template<class Component>
void GameObject::AddComponent() {
    m_components.emplace_back(std::make_shared<Component>(*this));
    m_components.back()->BaseInit();
}

void GameObject::RemoveComponent(Component& c) {

}




