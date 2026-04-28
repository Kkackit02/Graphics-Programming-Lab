#include <vector>
#include <chrono>

#ifdef WINDOWS  // 윈도우즈에서 컴파일 할때는 아래를 포함
#include <GL/glew.h>
#include <GL/freeglut.h>
#else
#include <OpenGL/gl3.h>
#include <GLut/glut.h>
#endif

#include <glm/glm.hpp>
#include "Geometry.h"

#include "Scene.h"

std::vector <Geometry*> g_Scene;
