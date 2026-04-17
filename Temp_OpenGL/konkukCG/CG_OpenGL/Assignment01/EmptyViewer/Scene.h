#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Surface.h"
#include "Camera.h"
#include <glm/glm.hpp>

class Scene {
public:
    std::vector<Surface*> surfaces;
    Camera camera;

    Scene(const Camera& _camera); 
    bool trace(const Ray& ray, glm::vec3& hit_point, glm::vec3& normal, Surface*& hit_surface, float& t_hit) const;
};

#endif