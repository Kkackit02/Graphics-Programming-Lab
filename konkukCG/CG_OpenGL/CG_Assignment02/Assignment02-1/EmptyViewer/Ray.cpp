#include "Ray.h"
#include <glm/glm.hpp>

Ray::Ray(glm::vec3 o, glm::vec3 d) : origin(o), direction(glm::normalize(d)) {}

glm::vec3 Ray::getTransform(float t) const {
    return origin + t * direction;
}