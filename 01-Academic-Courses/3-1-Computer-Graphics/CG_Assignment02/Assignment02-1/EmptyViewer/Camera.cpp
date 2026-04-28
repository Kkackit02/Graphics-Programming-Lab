#include "Camera.h"
#include "Light.h"
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(glm::vec3 _pos, glm::vec3 _lookAt, glm::vec3 _up, float _l, float _r, float _b, float _t, float _d, int _nx, int _ny)
    : position(_pos), l(_l), r(_r), b(_b), t(_t), d(_d), nx(_nx), ny(_ny)
{
    // 카메라 방향 벡터 설정
    w = glm::normalize(position - _lookAt); // 카메라가 보는 방향 (뒤쪽)
    u = glm::normalize(glm::cross(_up, w)); // 오른쪽 방향
    v = glm::cross(w, u);                   // 위쪽 방향

    // 종횡비 보정
    float aspect_ratio = float(nx) / float(ny);
    r *= aspect_ratio;
    l *= aspect_ratio;
}

Ray Camera::getRay(int i, int j) const {
    float u_s = l + (r - l) * (i + 0.5f) / nx;
    float v_s = b + (t - b) * (j + 0.5f) / ny;

    glm::vec3 pixel_position = position - d * w + u_s * u + v_s * v;
    glm::vec3 ray_direction = glm::normalize(pixel_position - position);

    return Ray(position, ray_direction);
}
