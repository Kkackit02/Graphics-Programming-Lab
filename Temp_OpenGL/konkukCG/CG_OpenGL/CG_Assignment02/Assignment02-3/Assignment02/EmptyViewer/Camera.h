#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "Ray.h"

class Camera {
public:
    
     glm::vec3 position; // 카메라 위치
      glm::vec3 u, v, w;  // 카메라 좌표축 (오른쪽, 위쪽, 바라보는 방향)
      float l, r, b, t, d; // 뷰잉 프러스트럼 정의
      int nx, ny; // 이미지 해상도

      Camera(glm::vec3 _pos, glm::vec3 _lookAt, glm::vec3 _up, float _l, float _r, float _b, float _t, float _d, int _nx, int _ny);
      Ray getRay(int i, int j) const;
};

#endif 