
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "load_mesh.h"
using namespace std;
using namespace glm;

int Width = 1280;
int Height = 1280;


float  					gTotalTimeElapsed = 0;
int 					gTotalFrames = 0;
GLuint 					gTimer;

void init_timer()
{
    glGenQueries(1, &gTimer);
}

void start_timing()
{
    glBeginQuery(GL_TIME_ELAPSED, gTimer);
}

float stop_timing()
{
    glEndQuery(GL_TIME_ELAPSED);

    GLint available = GL_FALSE;
    while (available == GL_FALSE)
        glGetQueryObjectiv(gTimer, GL_QUERY_RESULT_AVAILABLE, &available);

    GLint result;
    glGetQueryObjectiv(gTimer, GL_QUERY_RESULT, &result);

    float timeElapsed = result / (1000.0f * 1000.0f * 1000.0f);
    return timeElapsed;
}


struct Vertex {
    vec3 pos, normal;
};

vector<Vertex> gVertices;
vector<int> gIndexBuffer;
int gNumVertices = 0, gNumTriangles = 0;

void render_immediate() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Camera 설정
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 1000.0);  // 과제 조건의 시야 범위

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 0,   // eye
        0, 0, -1,  // center
        0, 1, 0);  // up
    vec3 dir = normalize(vec3(1.0f, 1.0f, 1.0f));
    //light Source = (-1,-1,-1) -> Direction to (1,1,1)
    float lightDir[] = { dir.x, dir.y, dir.z, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightDir);
    glTranslatef(0.1f, -1.0f, -1.5f);
    glScalef(10.0f, 10.0f, 10.0f);

    // Directional light 설정: (w = 0)

    glBegin(GL_TRIANGLES);
    for (int tri = 0; tri < gNumTriangles; ++tri) {
        int i0 = gIndexBuffer[3 * tri + 0];
        int i1 = gIndexBuffer[3 * tri + 1];
        int i2 = gIndexBuffer[3 * tri + 2];

        for (int i : { i0, i1, i2 }) {
            vec3 n = gVertices[i].normal;
            vec3 v = gVertices[i].pos;
            glNormal3f(n.x, n.y, n.z);
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();
}

void resize_callback(GLFWwindow*, int nw, int nh)
{
    Width = nw;
    Height = nh;
    glViewport(0, 0, nw, nh);
}

int main(int argc, char* argv[])
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(Width, Height, "CG Assignment08-Q1 - Bunny", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // backface culling OFF


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    float globalAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    float ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float specularLight[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // no specular

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0f, 1.0f, 1.0f); // ka = kd = (1,1,1)
    float matSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);  // p = 0


    glfwSetFramebufferSizeCallback(window, resize_callback);
    resize_callback(window, Width, Height);

    // load bunny
    load_mesh("bunny.obj");
    gNumTriangles = gTriangles.size();
    gNumVertices = gPositions.size();
    gVertices.resize(gNumVertices);
    gIndexBuffer.resize(gNumTriangles * 3);

    for (size_t i = 0; i < gNumVertices; ++i) {
        gVertices[i].pos = vec3(gPositions[i].x, gPositions[i].y, gPositions[i].z);
        gVertices[i].normal = vec3(gNormals[i].x, gNormals[i].y, gNormals[i].z);
    }

    for (size_t i = 0; i < gNumTriangles; ++i) {
        gIndexBuffer[i * 3 + 0] = gTriangles[i].indices[0];
        gIndexBuffer[i * 3 + 1] = gTriangles[i].indices[1];
        gIndexBuffer[i * 3 + 2] = gTriangles[i].indices[2];
    }

    init_timer();
    // 메인 루프
    while (!glfwWindowShouldClose(window)) {

		start_timing();
        render_immediate();

        glfwSwapBuffers(window);
        glfwPollEvents();


        float timeElapsed = stop_timing();
        gTotalFrames++;
        gTotalTimeElapsed += timeElapsed;
        float fps = gTotalFrames / gTotalTimeElapsed;
        char string[1024] = { 0 };
        sprintf(string, "OpenGL Bunny_Q1: %0.2f FPS", fps);
        glfwSetWindowTitle(window, string);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
