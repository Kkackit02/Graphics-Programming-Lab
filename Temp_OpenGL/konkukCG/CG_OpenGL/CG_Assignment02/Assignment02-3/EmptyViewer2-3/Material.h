#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>


class Material {
public:
    glm::vec3 ka, kd, ks;
    float specularPower;

    Material(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, float sh)
        : ka(a), kd(d), ks(s), specularPower(sh) {
    }
};


#endif 