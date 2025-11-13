#pragma once

#include <memory>


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Component.h>
#include <Utilities/Log.hpp>

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