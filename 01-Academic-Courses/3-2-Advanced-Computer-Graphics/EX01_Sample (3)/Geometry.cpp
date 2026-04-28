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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "geometry.h"

Geometry::Geometry(GLuint activeProgram)
{    
	m_World = glm::mat4(1.0f);
    InitGL();
    ChangeProgramGL(activeProgram);
    
}

Geometry::~Geometry()
{
    DeleteGL();
}

void Geometry::SetPosition(glm::vec3 pos)
{
	m_World = glm::translate(m_World, pos);	
}



GLfloat cubeVertices[] = {
	0.1f, 0.1f,-0.1f, // triangle 2 : begin
	-0.1f,-0.1f,-0.1f,
	-0.1f, 0.1f,-0.1f, // triangle 2 : end
	0.1f, 0.1f,-0.1f,
	0.1f,-0.1f,-0.1f,
	-0.1f,-0.1f,-0.1f,

	-0.1f,-0.1f,-0.1f, // triangle 1 : begin
	-0.1f,-0.1f, 0.1f,
	-0.1f, 0.1f, 0.1f, // triangle 1 : end
	-0.1f,-0.1f,-0.1f,
	-0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f,-0.1f,

	-0.1f, 0.1f, 0.1f,
	-0.1f,-0.1f, 0.1f,
	0.1f,-0.1f, 0.1f,
	0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f, 0.1f,
	0.1f,-0.1f, 0.1f,

	0.1f, 0.1f, 0.1f,
	0.1f,-0.1f,-0.1f,
	0.1f, 0.1f,-0.1f,
	0.1f,-0.1f,-0.1f,
	0.1f, 0.1f, 0.1f,
	0.1f,-0.1f, 0.1f,

	0.1f,-0.1f, 0.1f,
	-0.1f,-0.1f,-0.1f,
	0.1f,-0.1f,-0.1f,
	0.1f,-0.1f, 0.1f,
	-0.1f,-0.1f, 0.1f,
	-0.1f,-0.1f,-0.1f,

	0.1f, 0.1f, 0.1f,
	0.1f, 0.1f,-0.1f,
	-0.1f, 0.1f,-0.1f,
	0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f,-0.1f,
	-0.1f, 0.1f, 0.1f,
};

GLfloat cubeColors[] = {
	1.0, 0.0, 0.0, 1.0,
	1.0, 0.0, 0.0, 1.0,
	1.0, 0.0, 0.0, 1.0,
	1.0, 0.0, 0.0, 1.0,
	1.0, 0.0, 0.0, 1.0,
	1.0, 0.0, 0.0, 1.0,

	0.0, 1.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,

	0.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 1.0,

	1.0, 1.0, 0.0, 1.0,
	1.0, 1.0, 0.0, 1.0,
	1.0, 1.0, 0.0, 1.0,
	1.0, 1.0, 0.0, 1.0,
	1.0, 1.0, 0.0, 1.0,
	1.0, 1.0, 0.0, 1.0,

	1.0, 0.0, 1.0, 1.0,
	1.0, 0.0, 1.0, 1.0,
	1.0, 0.0, 1.0, 1.0,
	1.0, 0.0, 1.0, 1.0,
	1.0, 0.0, 1.0, 1.0,
	1.0, 0.0, 1.0, 1.0,

	0.0, 1.0, 1.0, 1.0,
	0.0, 1.0, 1.0, 1.0,
	0.0, 1.0, 1.0, 1.0,
	0.0, 1.0, 1.0, 1.0,
	0.0, 1.0, 1.0, 1.0,
	0.0, 1.0, 1.0, 1.0,
};

void Geometry::CreateCube()
{
	m_GLMode = GL_TRIANGLES;
	m_NumVertices = sizeof(cubeVertices) / (3 * sizeof(float));
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOIDs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);	
}

void Geometry::Draw()
{
	DrawGL();
}

void Geometry::AnimateRotate(bool start)
{
	if (start == true)
	{
		m_AnimTime = std::chrono::high_resolution_clock::now();
	}
	else
	{
		std::chrono::high_resolution_clock::time_point curTime = std::chrono::high_resolution_clock::now();

		auto passedMS = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - m_AnimTime);
		long passed = passedMS.count();
		if (passed > 100) // if longer than 100 ms has passed
		{
			int animAmount = passed / 100;
			m_World = glm::rotate(m_World, animAmount * 0.1f, glm::vec3(1.0f, 1.0f, 1.0f));
			m_AnimTime = curTime;
		}
	}
}



void Geometry::InitGL()
{
	glGenVertexArrays(1, &m_VAOID);
	glGenBuffers(2, m_VBOIDs);
}
void Geometry::DeleteGL()
{
	glDeleteVertexArrays(1, &m_VAOID);
	glDeleteBuffers(2, m_VBOIDs);
}
void Geometry::ChangeProgramGL(GLuint activeProgram)
{
	m_ProgramID = activeProgram;
	glBindVertexArray(m_VAOID);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOIDs[0]);
	GLuint posAttribLoc = glGetAttribLocation(m_ProgramID, "inPos");
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(posAttribLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOIDs[1]);
	GLuint colAttribLoc = glGetAttribLocation(m_ProgramID, "inColor");
	glVertexAttribPointer(colAttribLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(colAttribLoc);
}
void Geometry::DrawGL()
{
	GLuint wmatID = glGetUniformLocation(m_ProgramID, "wMat");
	glUniformMatrix4fv(wmatID, 1, GL_FALSE, glm::value_ptr(m_World));;

	glBindVertexArray(m_VAOID);
	glDrawArrays(m_GLMode, 0, m_NumVertices);
}