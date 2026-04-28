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
	//glBegin(GL_TRIANGLES);
	glBegin(GL_LINE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(-5, -5, 0);
	glVertex3f(5, -5, 0);
	glVertex3f(10, 5, 0);
	glVertex3f(15, -5, 0);
	glVertex3f(20, 5, 0);
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
