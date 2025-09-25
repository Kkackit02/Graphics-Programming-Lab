#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/glm.hpp>  // vec3 타입 쓰려면 필요
#include "camera.h"
#include <cmath>  
#include <cstdio> 
typedef struct {
    glm::vec3 ka;
    glm::vec3 ks;
    glm::vec3 kd;
    float p;
    float Ia;

}Material;
#endif 