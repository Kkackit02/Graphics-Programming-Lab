#pragma once
// Light.h
#pragma once
#include <glm/glm.hpp>

class Light {
public:
    glm::vec3 position;   // ±¤¿øÀÇ À§Ä¡
    glm::vec3 intensity;  // RGB ±¤ ¼¼±â (¿¹: vec3(1,1,1) = white)

    Light(const glm::vec3& pos, const glm::vec3& inten)
        : position(pos), intensity(inten) {
    }
};
