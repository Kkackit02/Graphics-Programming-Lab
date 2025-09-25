#include "Sphere.h"
#include <glm/gtc/constants.hpp>




bool Sphere::intersect(const Ray& ray, float& t_hit, glm::vec3& hit_point, glm::vec3& normal) const {
    glm::vec3 oc = ray.origin - center;
    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) {
        return false;
    }

    float sqrt_disc = std::sqrt(discriminant);
    float t1 = (-b - sqrt_disc) / (2.0f * a);
    float t2 = (-b + sqrt_disc) / (2.0f * a);

    float t = -1.0f;
    if (t1 > 0.001f && t2 > 0.001f) {
        t = fmin(t1, t2);
    }
    else if (t1 > 0.001f) {
        t = t1;
    }
    else if (t2 > 0.001f) {
        t = t2;
    }

    if (t < 0.001f) return false;

    t_hit = t;
    hit_point = ray.origin + t_hit * ray.direction;
    normal = glm::normalize(hit_point - center);

    return true;
}