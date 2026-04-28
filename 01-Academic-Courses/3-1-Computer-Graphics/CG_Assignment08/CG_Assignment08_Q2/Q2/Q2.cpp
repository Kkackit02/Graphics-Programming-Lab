
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

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

#include "load_mesh.h"
using namespace glm;
#define M_PI 3.14159265358979323846


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




GLuint LoadShaders(const std::string& vertex_file_path, const std::string& fragment_file_path)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path.c_str(), std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("Impossible to open %s. Are you in the right directory? Don't forget to read the FAQ!\n", vertex_file_path.c_str());
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path.c_str(), std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader: %s\n", vertex_file_path.c_str());
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path.c_str());
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

void Update_mesh(const GLuint& VAO, const std::vector<GLuint>& GLBuffers,
    std::vector<glm::vec3> Positions,
    std::vector<glm::vec3> Normals,
    std::vector<glm::vec3> Colors,
    std::vector<unsigned int>& Indices)
{
    glBindVertexArray(VAO); // Must be bound before binding buffers

    glBindBuffer(GL_ARRAY_BUFFER, GLBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(Positions[0]), &Positions[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, GLBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(Normals[0]), &Normals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, GLBuffers[2]);
    glBufferData(GL_ARRAY_BUFFER, Colors.size() * sizeof(Colors[0]), &Colors[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLBuffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(Indices[0]), &Indices[0], GL_STATIC_DRAW);

    //glEnableVertexAttribArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO to avoid accidental modification
    glBindVertexArray(0); // Unbind VAO to avoid accidental modification
}

// --------------------------------------------------
// Global Variables
// --------------------------------------------------
int Width = 1280;
int Height = 1280;
// --------------------------------------------------

void resize_callback(GLFWwindow*, int nw, int nh)
{
    Width = nw;
    Height = nh;
    // Tell the viewport to use all of our screen estate
    glViewport(0, 0, nw, nh);
}

void processInput(GLFWwindow* window)
{
    // Close when the user hits 'q' or escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
        || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char* argv[])
{
    //
    // Initialize Window
    //


    GLFWwindow* window;


    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(Width, Height, "OpenGL Viewer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resize_callback);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
        return -1; 
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // 기본값, 백페이스 제거
    glFrontFace(GL_CCW); // 반시계방향이 앞면으로 취급됨
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Create and compile our GLSL program from the shaders
    GLuint shaderProgram = LoadShaders("Shader.vs", "Shader.fs");




    init_timer();
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    const int numBuffers = 4; // Buffers for Positions, Normals, Colors, Indices
    std::vector<GLuint> GLBuffers;
    GLBuffers.resize(numBuffers, 0);
    glGenBuffers(numBuffers, &GLBuffers[0]);
    std::vector<glm::vec3> Positions;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec3> Colors;
    std::vector<unsigned int> Indices;

    load_mesh("bunny.obj"); 

    for (size_t i = 0; i < gPositions.size(); ++i) {
        Positions.emplace_back(gPositions[i].x, gPositions[i].y, gPositions[i].z);
        Normals.emplace_back(gNormals[i].x, gNormals[i].y, gNormals[i].z);
        Colors.emplace_back(0.0f, 1.0f, 0.0f);
    }

    for (size_t i = 0; i < gTriangles.size(); ++i) {
        Indices.push_back(gTriangles[i].indices[0]);
        Indices.push_back(gTriangles[i].indices[1]);
        Indices.push_back(gTriangles[i].indices[2]);
    }
    Update_mesh(VAO, GLBuffers, Positions, Normals, Colors, Indices);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        start_timing();
        // input
        processInput(window);

        // render
        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(shaderProgram);

        // update uniform


    // 행렬 유니폼

         // 조명 위치와 색상
        vec4 lightPos = vec4(-1.0f, -1.0f, -1.0f , 0.0f);
        vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

        // 조명 유니폼
        glUniform4fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, value_ptr(lightColor));

        // ambient light
        vec3 Ia = vec3(0.2f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "Ia"), 1, value_ptr(Ia));

        // 재질 계수
        vec3 ka = vec3(1.0f, 1.0f, 1.0f);
        vec3 kd = vec3(1.0f, 1.0f, 1.0f);
        vec3 ks = vec3(0.0f);
        float shininess = 0.0f;

        glUniform3fv(glGetUniformLocation(shaderProgram, "ka"), 1, value_ptr(ka));
        glUniform3fv(glGetUniformLocation(shaderProgram, "kd"), 1, value_ptr(kd));
        glUniform3fv(glGetUniformLocation(shaderProgram, "ks"), 1, value_ptr(ks));
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);


        mat4 modeling = translate(mat4(1.0f), vec3(0.1f, -1.0f, -1.5f)) * scale(mat4(1.0f), vec3(10.0f));
        mat4 modeling_inv_tr = transpose(inverse(modeling));

        vec3 eye = vec3(0, 0, 0);
        glUniform3fv(glGetUniformLocation(shaderProgram, "eyePos"), 1, value_ptr(eye));
        vec3 center = vec3(0, 0, -1);  // e - w
        vec3 up = vec3(0, 1, 0);       // v
        mat4 camera = lookAt(eye, center, up);
        mat4 projection = frustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 1000.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modeling"), 1, GL_FALSE, value_ptr(modeling));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modeling_inv_tr"), 1, GL_FALSE, value_ptr(modeling_inv_tr));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "camera"), 1, GL_FALSE, value_ptr(camera));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));


        //glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(modeling));

        glBindVertexArray(VAO);
        // Draw the triangle!
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // no need to unbind it every time
        
    

        // TODO: Draw the bunny.

        float timeElapsed = stop_timing();
        gTotalFrames++;
        gTotalTimeElapsed += timeElapsed;
        float fps = gTotalFrames / gTotalTimeElapsed;
        char string[1024] = { 0 };
        sprintf(string, "OpenGL Bunny_Q2: %0.2f FPS", fps);
        glfwSetWindowTitle(window, string);


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Cleanup VAO
    glDeleteVertexArrays(1, &VAO);
    if (GLBuffers[0] != 0)
        glDeleteBuffers(numBuffers, &GLBuffers[0]);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}