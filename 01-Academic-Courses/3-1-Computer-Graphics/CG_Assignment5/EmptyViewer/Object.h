#ifndef OBJECT_H
#define OBJECT_H

#include "camera.h"
#include <cmath>  // C++ 표준 수학 라이브러리
#include <cstdio> // C++ 표준 입출력 (필요 시)
typedef struct {
    float* vertexBuffer;
    int* indexBuffer;
    int numVertices;
    int numTriangles;
}ObjectData; 
#endif