#ifndef PLANE_H
#define PLANE_H

#include "Surface.h"

class Plane : public Surface {
public:
    glm::vec3 point;  // 평면 위의 한 점
    glm::vec3 normal; // 평면의 법선 벡터


    Plane(const Material& _material, glm::vec3 _point, glm::vec3 _normal)
    : Surface(_material), point(_point) , normal(_normal)
    { }

    virtual bool intersect(const Ray& ray, float& t_hit, glm::vec3& hit_point, glm::vec3& normal_out) const override;
};

#endif 