// Student ID 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <chrono>
#include <sstream> // Added

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OBJ_Loader.h" // openSource 
#include "geometry.h"
#include "Scene.h"
#include "Spline.h" // Added

using namespace std;

// Globals
int g_windowWidth, g_windowHeight;
GLuint programID;
glm::mat4 projection, view;
// // This was in Scene.h/cpp, but let's define it here to be safe.
std::vector<glm::vec3> g_controlPoints; // Added
Spline* g_spline = nullptr; // Added
float g_splineAnimT = 0.0f;

// Forward declarations
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path);
void loadControlPoints(const char* filepath);

// Shader Loading
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

// Control Point Loading (Added)
void loadControlPoints(const char* filepath)
{
    g_controlPoints.clear();
    ifstream fileStream(filepath, ios::in);
    if (fileStream.is_open())
    {
        string line;
        while (getline(fileStream, line))
        {
            glm::vec3 point;
            stringstream ss(line);
            ss >> point.x >> point.y >> point.z;
            g_controlPoints.push_back(point);
        }
        fileStream.close();
        printf("Loaded %d control points from %s.\n", (int)g_controlPoints.size(), filepath);
    }
    else
    {
        printf("Failed to open control points file: %s\n", filepath);
    }
}

// Rendering
void renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    // Draw main objects
    glUseProgram(programID);
    GLuint projID = glGetUniformLocation(programID, "projection");
    GLuint viewID = glGetUniformLocation(programID, "view");
    glUniformMatrix4fv(projID, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(view));
	for (int idx = 0; idx < getScene().size(); idx++)
	{
		getScene()[idx]->Draw();
	}

    // Draw spline (Added)
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
	
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");

    projection = glm::perspective(glm::radians(45.0f), (float)g_windowWidth / (float)g_windowHeight, 0.1f, 1000.0f);
    view = glm::lookAt(
        glm::vec3(0, 5, 15),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

	// Load Piggy
	objl::Loader piggyLoader;
	if (piggyLoader.LoadFile("PiggyBank.obj"))
	{
		for (int i = 0; i < piggyLoader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = piggyLoader.LoadedMeshes[i];
			Geometry* geometryObject = new Geometry(programID);
			geometryObject->InitFromMesh(curMesh);
			geometryObject->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
			getScene().push_back(geometryObject);
		}
	}
	else { cout << "Failed to Load Pig" << endl; }

	// Load Cubes
	objl::Loader cubeLoader;
	if (cubeLoader.LoadFile("cube.obj"))
	{
		for (int i = 0; i < cubeLoader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = cubeLoader.LoadedMeshes[i];
			Geometry* geometryObject1 = new Geometry(programID);
			geometryObject1->InitFromMesh(curMesh);
			geometryObject1->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
			getScene().push_back(geometryObject1);
			Geometry* geometryObject2 = new Geometry(programID);
			geometryObject2->InitFromMesh(curMesh);
			geometryObject2->SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
			getScene().push_back(geometryObject2);
		}
	}
	else { cout << "Failed to load Cube" << endl; }

    // Load control points and create spline (Added)
	loadControlPoints("spline_control_points.txt");
	if (!g_controlPoints.empty())
	{
		g_spline = new Spline(g_controlPoints);
	}
}

// Animation
void Animate(int value)
{
    // Animate piggy on spline
    if (g_spline != nullptr && getScene().size() > 0)
    {
        g_splineAnimT += 0.001f; // Adjust speed here
        if (g_splineAnimT > 1.0f)
        {
            g_splineAnimT = 0.0f;
        }
        glm::vec3 newPos = g_spline->getPointOnSpline(g_splineAnimT);
        getScene()[0]->SetPosition(newPos);
        getScene()[0]->AnimateRotate(); // Also apply rotation
    }

	for (int idx = 1; idx < getScene().size(); idx++) // Start from 1 to skip piggy
	{
		if (idx == 1) { getScene()[idx]->AnimateTranslate(); }
		else if (idx == 2) { getScene()[idx]->AnimateScale(); }
		else { getScene()[idx]->AnimateRotate(); }
	}
	glutTimerFunc(10, Animate, 1);
	glutPostRedisplay();
}

// Main
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(200, 200);
    g_windowWidth = 480;
    g_windowHeight = 480;
	glutInitWindowSize(g_windowWidth, g_windowHeight);
	glutCreateWindow("Homework2 : Spline");

	init();

	glutTimerFunc(10, Animate, 1);
	glutDisplayFunc(renderScene);

	glutMainLoop();

    // Cleanup
	if (g_spline != nullptr) { delete g_spline; }
	for (int idx = 0; idx < getScene().size(); idx++)
	{
		delete getScene()[idx];
	}
	
	return 1;
}