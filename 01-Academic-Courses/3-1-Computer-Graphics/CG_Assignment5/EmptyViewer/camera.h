#ifndef CAMERA_H
#define CAMERA_H

#include "camera.h"
#include <cmath>  // C++ 표준 수학 라이브러리
#include <cstdio> // C++ 표준 입출력 (필요 시)
typedef struct {
    float eye[3]; // 카메라 위치
    float u[3];   // 오른쪽
    float v[3];   // 위쪽
    float w[3];   // 뒤쪽
} Camera;

#endif