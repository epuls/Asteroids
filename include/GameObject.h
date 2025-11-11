#pragma once

#include <vector>
#include <Component.h>

class GameObject{
public:
    void Init();
    void Start();
    void Update();
    void FixedUpdate();
    void LateUpdate();
    void OnDestroy();

    template<class Component>
    void AddComponent();
    void RemoveComponent(Component& c);

    GameObject(){ Init(); }

private:
    unsigned int m_idx = 0;
    std::vector<std::shared_ptr<Component>> m_components{};
};