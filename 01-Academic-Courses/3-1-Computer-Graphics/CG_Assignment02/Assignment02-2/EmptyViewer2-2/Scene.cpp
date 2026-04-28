#include "Scene.h"
#include <limits>
#include "Plane.h"
#include "Sphere.h"
#include "Light.h"


Scene::Scene(const Camera& _camera) : camera(_camera) {}

glm::vec3 Scene::trace(const Ray& ray, glm::vec3& hit_point, glm::vec3& normal, Surface*& hit_surface, Light*& light, float& t_hit) const {
    float closest_t = std::numeric_limits<float>::max();
    hit_surface = nullptr;
	glm::vec3 color = glm::vec3(0.0f);

    for (const auto& surface : surfaces) {
        float temp_t;
        glm::vec3 temp_hit_point, temp_normal;

        if (surface->intersect(ray, temp_t, temp_hit_point, temp_normal) && temp_t > 0.001f) {
            if (temp_t < closest_t) {
                closest_t = temp_t;
                hit_surface = surface;
                hit_point = temp_hit_point;
                normal = temp_normal;
                t_hit = closest_t; 
                color = surface->shade(ray, hit_point, normal, *light, surfaces);
            }
        }
    }
    return color;
}