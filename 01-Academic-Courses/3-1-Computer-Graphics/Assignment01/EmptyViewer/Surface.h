#ifndef SURFACE_H
#define SURFACE_H

#include "Ray.h"
#include <glm/glm.hpp>

class Surface {
public:
    virtual bool intersect(const Ray& ray, float& t_hit, glm::vec3& hit_point, glm::vec3& normal) const = 0;
};

#endif 