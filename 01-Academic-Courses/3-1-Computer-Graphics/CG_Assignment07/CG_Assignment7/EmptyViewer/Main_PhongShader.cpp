#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <sstream>
#include <string>
using namespace glm;
using namespace std;
// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 512;
int Height = 512;

GLuint program, vao, vbo, ebo;
vector<unsigned int> sphereIndices;

string LoadShaderSource(const char* filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Failed to open shader file: " << filepath << endl;
        exit(EXIT_FAILURE);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void checkShaderError(GLuint shader, const string& name) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        cerr << name << " compile error:\n" << log << endl;
    }
}

void checkProgramError(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        cerr << "Program link error:\n" << log << endl;
    }
}

void InitShaderAndBuffers() {
    glewInit();

    // 셰이더 컴파일
    string vsCode = LoadShaderSource("Phong.vert");
    string fsCode = LoadShaderSource("Phong.frag");
    const char* vs = vsCode.c_str();
    const char* fs = fsCode.c_str();

    GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsId, 1, &vs, NULL);
    glCompileShader(vsId);
    checkShaderError(vsId, "Phong.vert");

    GLuint fsId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fsId, 1, &fs, NULL);
    glCompileShader(fsId);
    checkShaderError(fsId, "Phong.frag");

    program = glCreateProgram();
    glAttachShader(program, vsId);
    glAttachShader(program, fsId);
    glLinkProgram(program);
    checkProgramError(program);
    glUseProgram(program);


    // 구 정점 생성
    vector<float> sphereVertices;
    const int sectorCount = 32;
    const int stackCount = 16;
    const float radius = 1.0f;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = pi<float>() / 2 - i * pi<float>() / stackCount;
        float xy = radius * cos(stackAngle);
        float z = radius * sin(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * pi<float>() / sectorCount;
            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);

            // position
            sphereVertices.push_back(x);
            sphereVertices.push_back(y);
            sphereVertices.push_back(z);
            sphereVertices.push_back(1.0f);

            // normal
            vec3 n = normalize(vec3(x, y, z));
            sphereVertices.push_back(n.x);
            sphereVertices.push_back(n.y);
            sphereVertices.push_back(n.z);
            sphereVertices.push_back(0.0f);
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            sphereIndices.push_back(k1);
            sphereIndices.push_back(k2);
            sphereIndices.push_back(k1 + 1);

            sphereIndices.push_back(k1 + 1);
            sphereIndices.push_back(k2);
            sphereIndices.push_back(k2 + 1);
        }
    }



    // 조명 위치와 색상
    vec3 lightPos = vec3(-4.0f, 4.0f, -3.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

    // 조명 유니폼
    glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(program, "lightColor"), 1, value_ptr(lightColor));

    // ambient light
    vec3 Ia = vec3(0.2f);
    glUniform3fv(glGetUniformLocation(program, "Ia"), 1, value_ptr(Ia));

    // 재질 계수
    vec3 ka = vec3(0.0f, 1.0f, 0.0f);
    vec3 kd = vec3(0.0f, 0.5f, 0.0f);
    vec3 ks = vec3(0.5f);
    float shininess = 32.0f;

    glUniform3fv(glGetUniformLocation(program, "ka"), 1, value_ptr(ka));
    glUniform3fv(glGetUniformLocation(program, "kd"), 1, value_ptr(kd));
    glUniform3fv(glGetUniformLocation(program, "ks"), 1, value_ptr(ks));
    glUniform1f(glGetUniformLocation(program, "shininess"), shininess);



    // VAO/VBO/EBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

    // 행렬 유니폼
    mat4 modeling = translate(mat4(1.0f), vec3(0, 0, -7)) * scale(mat4(1.0f), vec3(2.0f));
    mat4 modeling_inv_tr = transpose(inverse(modeling));

    vec3 eye = vec3(0, 0, 0);
    vec3 center = vec3(0, 0, -1);  // e - w
    vec3 up = vec3(0, 1, 0);       // v
    mat4 camera = lookAt(eye, center, up);
    mat4 projection = frustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 1000.0f);

    glUniformMatrix4fv(glGetUniformLocation(program, "modeling"), 1, GL_FALSE, value_ptr(modeling));
    glUniformMatrix4fv(glGetUniformLocation(program, "modeling_inv_tr"), 1, GL_FALSE, value_ptr(modeling_inv_tr));
    glUniformMatrix4fv(glGetUniformLocation(program, "camera"), 1, GL_FALSE, value_ptr(camera));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, value_ptr(projection));
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
    glutSwapBuffers();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(Width, Height);
    glutCreateWindow("Sphere in Modern OpenGL");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    InitShaderAndBuffers();
    glutMainLoop();
    return 0;
}