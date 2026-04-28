#ifndef SURFACE_H
#define SURFACE_H

#include "Ray.h"
#include "Material.h"
#include <glm/glm.hpp>
#include "Surface.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include "Light.h"
#include <vector>

class Surface {
protected:
    Material material;  // 상속 클래스에서도 접근 가능하도록

public:
    Surface(const Material& m) : material(m) {}
    virtual ~Surface() = default;

    virtual bool intersect(const Ray& ray, float& t_hit, glm::vec3& hit_point, glm::vec3& normal) const = 0;

    Material getMaterial() const { return material; }

    virtual glm::vec3 shade(const Ray& ray,
        const glm::vec3& hit_point,
        const glm::vec3& normal,
        const Light& light,
        const std::vector<Surface*>& surfaces) const;
};

#endif
