#include <Game/Components/Transform.hpp>

void Transform::Translate() {


    m_dirty = true;
}

void Transform::Rotate() {

}


void Transform::SetPosition(glm::vec3 newPosition) {
    position = newPosition;
    m_dirty = true;
}