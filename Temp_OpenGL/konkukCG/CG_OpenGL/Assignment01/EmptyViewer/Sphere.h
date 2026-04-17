#ifndef SPHERE_H
#define SPHERE_H

#include "Surface.h"
#include <glm/glm.hpp>

class Sphere : public Surface {
public:
    glm::vec3 center;
    float radius;

    Sphere(glm::vec3 _center, float _radius);
    virtual bool intersect(const Ray& ray, float& t_hit, glm::vec3& hit_point, glm::vec3& normal) const override;
};

#endif 