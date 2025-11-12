#pragma once

#include <vector>
#include <Component.h>
#include <type_traits>

#include <unordered_map>
#include <deque>
#include <any>
#include <typeindex>
#include <stdexcept>

class GameObject{
public:
    void Init();
    void Start();
    void Update();
    void FixedUpdate();
    void LateUpdate();
    void OnDestroy();


    template<typename T, typename = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
    struct ComponentHandle{
        unsigned int index;
    };

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args) {
        auto& container = GetOrCreateContainer<T>();
        container.emplace_back(std::forward<Args>(args)...);
        T& component = container.back();
        component.gameObject = this;

        m_allComponents.push_back(&component);
        return component;
    }

    template<typename T>
    T* GetComponent() {
        auto it = m_components.find(std::type_index(typeid(T)));
        if (it == m_components.end())
            return nullptr;

        auto& container = std::any_cast<std::deque<T>&>(it->second);
        if (container.empty())
            return nullptr;

        return &container.front(); // or change if you allow multiple of same type
    }

    template<typename T>
    bool HasComponent() const {
        return m_components.contains(std::type_index(typeid(T)));
    }



    void RemoveComponent(Component& c);


    GameObject(){ Init(); }

private:
    unsigned int m_idx = 0;
    std::unordered_map<std::type_index, std::any> m_components{};
    std::vector<Component*> m_allComponents{};

    template<typename T>
    std::deque<T>& GetOrCreateContainer() {
        auto typeId = std::type_index(typeid(T));
        auto it = m_components.find(typeId);

        if (it == m_components.end()) {
            it = m_components.emplace(typeId, std::deque<T>{}).first;
        }

        return std::any_cast<std::deque<T>&>(it->second);
    }
};