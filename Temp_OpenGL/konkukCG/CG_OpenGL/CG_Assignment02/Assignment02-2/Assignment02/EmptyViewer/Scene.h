#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Surface.h"
#include "Camera.h"
#include "Light.h"
#include <glm/glm.hpp>

class Scene {
public:
    std::vector<Surface*> surfaces;
    std::vector<Light*> light;
    Camera camera;

    Scene(const Camera& _camera); 
    glm::vec3 trace(const Ray& ray, glm::vec3& hit_point, glm::vec3& normal, Surface*& hit_surface, Light*& _light, float& t_hit) const;
};

#endif