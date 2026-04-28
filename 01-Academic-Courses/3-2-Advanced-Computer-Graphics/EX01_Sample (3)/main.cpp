#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <chrono>

#define WINDOWS

#ifdef WINDOWS  // 윈도우즈에서 컴파일 할때는 아래를 포함
#include <GL/glew.h>
#include <GL/freeglut.h>
#else
#include <OpenGL/gl3.h>
#include <GLut/glut.h>
#endif

#include <glm/glm.hpp>

#include "geometry.h"
#include "Scene.h"

using namespace std;

GLuint myProgram;
int g_windowWidth, g_windowHeight;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if(VertexShaderStream.is_open())
	{
		string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0)
    {
	    vector<char> VertexShaderErrorMessage(InfoLogLength);
	    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
    }

	//Read the fragment shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if(FragmentShaderStream.is_open())
	{
		string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        vector<char> FragmentShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
    }
	//Link the program
	fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}


void renderScene(void)
{
	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//Let's draw something here
	
	//define the size of point and draw a point.
    
	for (int idx = 0; idx < g_Scene.size(); idx++)
	{
		// if each scene component is geometry
		g_Scene[idx]->Draw();
	}

	//Double buffer
	glutSwapBuffers();
}

void mousePoint(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        float nx, ny;
        nx = 2 * x / (float)(g_windowWidth-1) -1;
        ny = - 2* y / (float)(g_windowHeight-1) + 1;
        
		glm::vec3 pos(nx, ny, 0.0f);

		Geometry* newGeom = new Geometry(myProgram);
		newGeom->CreateCube();
		newGeom->SetPosition(pos);

		g_Scene.push_back(newGeom);
        
        glutPostRedisplay();
    }
}


void init()
{
#ifdef WINDOWS  // 윈도우즈에서 컴파일 할때는 아래를 포함
	//initilize the glew and check the errors.
    
	GLenum res = glewInit();
	if(res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
	}
#endif
	//select the background color
	
    glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);

}

void Animate(int value)
{
	for (int idx = 0; idx < g_Scene.size(); idx++)
	{
		// if each scene component is geometry
		g_Scene[idx]->AnimateRotate();
	}
	glutTimerFunc(10, Animate, 1);
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
#ifdef WINDOWS  // 윈도우즈에서 컴파일 할때는 아래를 포함
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA);
#endif
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
    
    g_windowWidth = 480;
    g_windowHeight = 480;
	glutInitWindowSize(g_windowWidth, g_windowHeight);
	//This is used to define the name of the window.
	glutCreateWindow("Simple OpenGL Window");

	//call initization function
	init();


	//3. 
	GLuint programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
	glUseProgram(programID);
    
    myProgram = programID;   
    
	glutMouseFunc(mousePoint);
	glutTimerFunc(10, Animate, 1);
	glutDisplayFunc(renderScene);

	//enter GLUT event processing cycle
	glutMainLoop();

	for (int idx = 0; idx < g_Scene.size(); idx++)
	{
		// if each scene component is geometry
		delete g_Scene[idx];
	}
	
	return 1;
}

