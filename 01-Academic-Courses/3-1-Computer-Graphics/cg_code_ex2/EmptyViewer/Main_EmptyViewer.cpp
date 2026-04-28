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
#include <glm/gtx/string_cast.hpp>

using namespace glm;

void reshape(int w, int h)
{
	glutPostRedisplay();
}
void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	//Start
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	glVertex2f(0, 0);
	glColor3f(0, 1, 0);
	glVertex2f(1, 0);
	glColor3f(0, 0, 1);
	glVertex2f(0, 1);
	glEnd();




	//End
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	// Initialize GLUT.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("OpenGL");
	// Set up GLUT callbacks.
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	// Start rendering.
	glutMainLoop();
	return 0;
}
