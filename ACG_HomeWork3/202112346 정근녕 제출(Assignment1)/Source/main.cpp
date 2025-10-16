#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <chrono>
#include <sstream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "OBJ_Loader.h" // openSource

#include "Object.h" 
#include "Scene.h"
#include "Spline.h"

using namespace std;

int g_windowWidth, g_windowHeight;
GLuint programID;
glm::mat4 projection, view;
std::vector<glm::vec3> controlPoints;
Spline* g_spline = nullptr; 
float g_splineAnimT = 0.0f;

GLuint LoadShadersSub(GLuint shader_type, const char* file_path)
{
    GLuint shaderID = glCreateShader(shader_type);
    GLint Result = GL_FALSE;
    int InfoLogLength;
    string ShaderCode;
    ifstream ShaderStream(file_path, ios::in);


    if (ShaderStream.is_open())
    {
        string Line = "";
        while (getline(ShaderStream, Line))
        {
            ShaderCode += "\n" + Line;
        }
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
    if (geometry_file_path != NULL) //pig, cube에는 사용을 안하므로..
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
    if (GeometryShaderID != 0)
    {
        glDeleteShader(GeometryShaderID);
    }
    return ProgramID;
}


void loadControlPoints(const char* filepath)
{
    controlPoints.clear();
    ifstream fileStream(filepath, ios::in);
    if (fileStream.is_open())
    {
        string line;
        while (getline(fileStream, line))
        {
            glm::vec3 point;
            stringstream ss(line);
            ss >> point.x >> point.y >> point.z;
            controlPoints.push_back(point);
        }
        fileStream.close();
        printf("Loaded %d control points from %s.\n", (int)controlPoints.size(), filepath);
    }
    else
    {
        printf("Failed to open control points file: %s\n", filepath);
    }
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);
    glUniform3f(glGetUniformLocation(programID, "LightPos"), 0.0f, 15.0f, 5.0f);
    glUniform3f(glGetUniformLocation(programID, "viewPos"), 0.0f, 5.0f, 15.0f);

    for (int idx = 0; idx < getScene().size(); idx++)
    {
        getScene()[idx]->Draw(view, projection);
    }

    if (g_spline != nullptr)
    {
        g_spline->Draw(view, projection);
    }

    glutSwapBuffers();
}

// Initialization
void init()
{
	GLenum res = glewInit();
	
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	programID = LoadShaders("VertexShader.txt", "FragmentShader.txt"); // 객체 쉐이더 load

    projection = glm::perspective(glm::radians(45.0f), (float)g_windowWidth / (float)g_windowHeight, 0.1f, 1000.0f);
    view = glm::lookAt(glm::vec3(0, 5, 15),glm::vec3(0, 0, 0),glm::vec3(0, 1, 0));

	// Load Piggy
	objl::Loader piggyLoader; 
	if (piggyLoader.LoadFile("PiggyBank.obj"))
	{
		for (int i = 0; i < piggyLoader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = piggyLoader.LoadedMeshes[i];
			Object* PigObject = new Object(programID);
            PigObject->InitFromMesh(curMesh);
            PigObject->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			getScene().push_back(PigObject);
		}
	}
	// Load Cubes
	objl::Loader cubeLoader;
	if (cubeLoader.LoadFile("cube.obj"))
	{
		for (int i = 0; i < cubeLoader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = cubeLoader.LoadedMeshes[i];
			Object* CubeObject1 = new Object(programID);
            CubeObject1->InitFromMesh(curMesh);
            CubeObject1->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));

            Object* CubeObject2 = new Object(programID);
            CubeObject2->InitFromMesh(curMesh);
            CubeObject2->SetPosition(glm::vec3(-2.0f, -4.0f, -5.0f));

            Object* CubeObject3 = new Object(programID);
            CubeObject3->InitFromMesh(curMesh);
            CubeObject3->SetPosition(glm::vec3(2.0f, 1.0f, -15.0f));


            getScene().push_back(CubeObject1);
            getScene().push_back(CubeObject2);
            getScene().push_back(CubeObject3);

		}
	}

	loadControlPoints("spline_control_points.txt"); //controlPoint 불러오기
	if (!controlPoints.empty())
	{
		g_spline = new Spline(controlPoints);
	}
}

void Animate(int value)
{
    if (g_spline != nullptr)
    {
        g_splineAnimT += 0.001f; // 속도 조절
        if (g_splineAnimT > 1.0f)
        {
            g_splineAnimT = 0.0f; //loop 될 수 있도록 수정
        }


        glm::vec3 newPos = g_spline->getPointOnSpline(g_splineAnimT);
        // 계산된 Spline Curve대로 움직이도록 애니메이션 적용
        if (getScene().size() > 1)
        {
            getScene()[1]->SetPosition(newPos);
            getScene()[4]->SetPosition(newPos);
            
        }
    }

    if (getScene().size() > 0)
    {
        for (int i = 0; i < getScene().size(); i++)
        {
            getScene()[i]->AnimateRotate_L(); //모든 Object 회전시키기
        }
    }
   
    glutTimerFunc(10, Animate, 1);
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(200, 200);
    g_windowWidth = 480;
    g_windowHeight = 480;
    glutInitWindowSize(g_windowWidth, g_windowHeight);
    glutCreateWindow("Assignment#1 202112346");
    init();

    glutTimerFunc(10, Animate, 1);
    glutDisplayFunc(renderScene);

    glutMainLoop();

    if (g_spline != nullptr) { delete g_spline; }
    for (int idx = 0; idx < getScene().size(); idx++)
    {
        delete getScene()[idx];
    }

    return 1;
}

