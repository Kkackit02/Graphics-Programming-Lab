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
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <sstream>
#include <string>
using namespace glm;

// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 1280;
int Height = 720;

GLuint program;
GLuint vao, vbo;

// -------------------------------------------------


std::string LoadShaderSource(const char* filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open shader file: " << filepath << std::endl;
		exit(EXIT_FAILURE);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / h, 1.0, 100.0);
}
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -7);

	// 조명 위치 (카메라 기준 좌표계)
	GLfloat light_pos[] = { -4.0f, 4.0f, 4.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	// 재질 색상: 녹색
	GLfloat mat_diffuse[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	// (선택) 스페큘러 반짝임 추가
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = { 30.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glutSolidSphere(2, 32, 16);
	glutSwapBuffers();
}


int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("OpenGL Sphere");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_FLAT);
	glutMainLoop();
	return 0;
}

	/*
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";
	*/

	/*
	std::string vertexCode = LoadShaderSource("vertex.glsl");
	std::string fragmentCode = LoadShaderSource("fragment.glsl");

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();

	// vsSource is a string containing the vertex shader source
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// fsSource is a string containing the fragment shader source
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);


	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glUseProgram(program);

	// 행렬 설정 및 uniform 전송
	mat4 modeling = mat4(1.0f);
	mat4 camera = lookAt(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 projection = perspective(radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

	glUniformMatrix4fv(glGetUniformLocation(program, "modeling"), 1, GL_FALSE, value_ptr(modeling));
	glUniformMatrix4fv(glGetUniformLocation(program, "camera"), 1, GL_FALSE, value_ptr(camera));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, value_ptr(projection));


	// 예: 3개의 정점 (각 정점당 4D 위치 + 4D 법선)
	float vertexData[] = {
		// position            // normal
		1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f
	};

	// VBO 생성
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// VAO 생성
	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	// 정점 위치 (index 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	// 정점 법선 (index 1)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	*/


