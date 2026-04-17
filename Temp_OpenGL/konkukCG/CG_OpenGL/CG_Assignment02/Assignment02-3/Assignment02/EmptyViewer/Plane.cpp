#include "Plane.h"
#include <glm/gtc/constants.hpp>
#include <cmath>


bool Plane::intersect(const Ray& ray, float& t_hit, glm::vec3& hit_point, glm::vec3& normal_out) const {
    float denominator = glm::dot(normal, ray.direction);

    if (fabs(denominator) < 0.001f) {
        return false;
    }

    float t = glm::dot(normal, (point - ray.origin)) / denominator;

    if (t < 0.001f) {
        
        return false;
    }

    t_hit = t;
    hit_point = ray.origin + t_hit * ray.direction;
    normal_out = normal; 

    return true; 
}
