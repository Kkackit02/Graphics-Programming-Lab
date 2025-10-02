#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <chrono>

//#ifdef WINDOWS  
#include <GL/glew.h>
#include <GL/freeglut.h>
/*
#else
#include <OpenGL/gl3.h>
#include <GLut/glut.h>
#endif
*/
#include <glm/glm.hpp>


#include "OBJ_Loader.h"
#include "geometry.h"
#include "Scene.h"

using namespace std;

GLuint myProgram;
int g_windowWidth, g_windowHeight;

GLuint programID ;
GLuint programIDFur ;

objl::Loader Loader;

// Load .obj File
bool loadout = Loader.LoadFile("cube.obj");


GLuint LoadShadersSub(GLuint shader_type, const char* file_path)
{
    GLuint shaderID = glCreateShader(shader_type);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    //Read the vertex shader code from the file
    string ShaderCode;
    ifstream ShaderStream(file_path, ios::in);
    if(ShaderStream.is_open())
    {
        string Line = "";
        while(getline(ShaderStream, Line))
            ShaderCode += "\n" + Line;
        ShaderStream.close();
    }

    //Compile Vertex Shader
    printf("Compiling shader : %s\n", file_path);
    char const* SourcePointer = ShaderCode.c_str();
    glShaderSource(shaderID, 1, &SourcePointer, NULL);
    glCompileShader(shaderID);

    //Check Vertex Shader
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
	//create the shaders
    GLuint VertexShaderID = 0;
    GLuint FragmentShaderID = 0;
    GLuint GeometryShaderID = 0;
    
	VertexShaderID = LoadShadersSub(GL_VERTEX_SHADER, vertex_file_path);
	FragmentShaderID = LoadShadersSub(GL_FRAGMENT_SHADER, fragment_file_path);
    
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
        glUseProgram(programID);
		g_Scene[idx]->Draw();
        glUseProgram(programIDFur);
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

	GLenum res = glewInit();
	
    glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);


	// If so continue
	if (loadout)
	{
		// Create/Open e1Out.txt
		std::ofstream file("e1Out.txt");

		// Go through each loaded mesh and out its contents
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			// Copy one of the loaded meshes to be our current mesh
			objl::Mesh curMesh = Loader.LoadedMeshes[i];

			// Print Mesh Name
			file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

			// Print Vertices
			file << "Vertices:\n";

			// Go through each vertex and print its number,
			//  position, normal, and texture coordinate
			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
				file << "V" << j << ": " <<
					"P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
					"N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
					"TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
			}

			// Print Indices
			file << "Indices:\n";

			// Go through every 3rd index and print the
			//	triangle that these indices represent
			for (int j = 0; j < curMesh.Indices.size(); j += 3)
			{
				file << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
			}

			// Print Material
			file << "Material: " << curMesh.MeshMaterial.name << "\n";
			file << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", " << curMesh.MeshMaterial.Ka.Z << "\n";
			file << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", " << curMesh.MeshMaterial.Kd.Z << "\n";
			file << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", " << curMesh.MeshMaterial.Ks.Z << "\n";
			file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
			file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
			file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
			file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
			file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
			file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
			file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
			file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
			file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";

			// Leave a space to separate from the next mesh
			file << "\n";
		}

		// Close File
		file.close();
	}
	// If not output an error
	else
	{
		// Create/Open e1Out.txt
		std::ofstream file("e1Out.txt");

		// Output Error
		file << "Failed to Load File. May have failed to find it or it was not an .obj file.\n";

		// Close File
		file.close();
	}
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
	//glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#endif
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
    
    g_windowWidth = 480;
    g_windowHeight = 480;
	glutInitWindowSize(g_windowWidth, g_windowHeight);

	//This is used to define the name of the window.
	glutCreateWindow("Homework2");


	//call initization function
	init();


	//3. 
	programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
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

