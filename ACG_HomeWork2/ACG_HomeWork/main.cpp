//202112346 정근녕
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <chrono>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OBJ_Loader.h" // openSource 사용


#include "geometry.h"
#include "Scene.h"

using namespace std;

int g_windowWidth, g_windowHeight;

GLuint programID;
glm::mat4 projection, view;



GLuint LoadShadersSub(GLuint shader_type, const char* file_path)
{
    GLuint shaderID = glCreateShader(shader_type);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    string ShaderCode;
    ifstream ShaderStream(file_path, ios::in);
    if(ShaderStream.is_open())
    {
        string Line = "";
        while(getline(ShaderStream, Line))
            ShaderCode += "\n" + Line;
        ShaderStream.close();
    }

    printf("Compiling shader : %s\n", file_path);
    char const* SourcePointer = ShaderCode.c_str();
    glShaderSource(shaderID, 1, &SourcePointer, NULL);
    glCompileShader(shaderID);

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0)
    {
        vector<char> ShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &ShaderErrorMessage[0]);
    }
    
    return shaderID;
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path = NULL)
{
    GLuint VertexShaderID = LoadShadersSub(GL_VERTEX_SHADER, vertex_file_path);
    GLuint FragmentShaderID = LoadShadersSub(GL_FRAGMENT_SHADER, fragment_file_path);
    GLuint GeometryShaderID = 0;
    if (geometry_file_path != NULL)
    {
        GeometryShaderID = LoadShadersSub(GL_GEOMETRY_SHADER, geometry_file_path);
    }

	fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    if (GeometryShaderID != 0)
    {
        glAttachShader(ProgramID, GeometryShaderID);
    }
    glLinkProgram(ProgramID);
 
    GLint Result = GL_FALSE;
    int InfoLogLength;
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glUseProgram(programID);

    //Shader에 projection, View 행렬 보내기
    GLuint projID = glGetUniformLocation(programID, "projection");
    GLuint viewID = glGetUniformLocation(programID, "view");
    glUniformMatrix4fv(projID, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(view));

	for (int idx = 0; idx < g_Scene.size(); idx++)
	{
		g_Scene[idx]->Draw();
	}

	glutSwapBuffers();
}

void init()
{
	GLenum res = glewInit();
	
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // 기본 배경 색 설정
	glEnable(GL_DEPTH_TEST);

	programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");

    projection = glm::perspective(glm::radians(45.0f), (float)g_windowWidth / (float)g_windowHeight, 0.1f, 1000.0f);
    view = glm::lookAt(
        glm::vec3(0, 5, 15), // 카메라 포지션
        glm::vec3(0, 0, 0),  // Lookat point
        glm::vec3(0, 1, 0)   // 상단 방향(Head)
    );

	// Load .obj File
	objl::Loader piggyLoader;
	if (piggyLoader.LoadFile("PiggyBank.obj"))
	{
		for (int i = 0; i < piggyLoader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = piggyLoader.LoadedMeshes[i];
			Geometry* geometryObject = new Geometry(programID);
            geometryObject->InitFromMesh(curMesh);
			g_Scene.push_back(geometryObject);
		}
	}
    else
    {
        cout << "Failed to Load Pig" << endl;
    }

	objl::Loader cubeLoader;
	if (cubeLoader.LoadFile("cube.obj"))
	{
		for (int i = 0; i < cubeLoader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = cubeLoader.LoadedMeshes[i];
			Geometry* geometryObject = new Geometry(programID);
            geometryObject->InitFromMesh(curMesh);
            geometryObject->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			g_Scene.push_back(geometryObject);
		}
	}
	else
	{
		cout << "Failed to load Cube" << endl;
	}
}

void Animate(int value)
{
	for (int idx = 0; idx < g_Scene.size(); idx++)
	{
		g_Scene[idx]->AnimateRotate();
	}
	glutTimerFunc(10, Animate, 1);
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
#ifdef WINDOWS
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#else
	//glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#endif
	glutInitWindowPosition(200, 200);
    
    g_windowWidth = 480;
    g_windowHeight = 480;
	glutInitWindowSize(g_windowWidth, g_windowHeight);

	glutCreateWindow("Homework2 : Cube, PiggyBank");

	init();

	glutTimerFunc(10, Animate, 1);
	glutDisplayFunc(renderScene);

	glutMainLoop();

	for (int idx = 0; idx < g_Scene.size(); idx++)
	{
		delete g_Scene[idx];
	}
	
	return 1;
}
