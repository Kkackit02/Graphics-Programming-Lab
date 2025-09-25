#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

class Ray {
public:
    glm::vec3 origin;    // 시작점
    glm::vec3 direction; // 방향

    Ray(glm::vec3 o, glm::vec3 d);
    glm::vec3 getTransform(float t) const;
};

#endif 