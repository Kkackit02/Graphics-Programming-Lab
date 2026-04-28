#ifndef SPHERE_H
#define SPHERE_H

#include "Surface.h"
#include <glm/glm.hpp>
#include "Material.h" // 추가된 헤더

class Sphere : public Surface {
public:
    glm::vec3 center;
    float radius;


    Sphere(const Material& _material, glm::vec3 _center, float _radius)
        : Surface(_material), center(_center), radius(_radius)
    { }
    virtual bool intersect(const Ray& ray, float& t_hit, glm::vec3& hit_point, glm::vec3& normal) const override;
};



#endif
