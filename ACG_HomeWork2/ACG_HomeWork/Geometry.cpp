#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <chrono>
#include <limits>
#include <algorithm>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "geometry.h"

Geometry::Geometry(GLuint activeProgram)
{    
	m_World = glm::mat4(1.0f);
    m_Y_RotationAngle = 0.0f;
    m_CenterOffset = glm::vec3(0.0f);
    m_TextureID = 0; // Initialize texture ID
    InitGL();
    ChangeProgramGL(activeProgram);
}

Geometry::~Geometry()
{
    DeleteGL();
}

void Geometry::SetPosition(glm::vec3 pos)
{
	m_World = glm::translate(glm::mat4(1.0f), pos);	
}

void Geometry::InitFromMesh(const objl::Mesh& mesh) // 메쉬 Load
{
	m_GLMode = GL_TRIANGLES;
	m_NumIndices = mesh.Indices.size();

	// 불러온 Material 저장
	m_Ka = glm::vec3(mesh.MeshMaterial.Ka.X, mesh.MeshMaterial.Ka.Y, mesh.MeshMaterial.Ka.Z);
	m_Kd = glm::vec3(mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z);
	m_Ks = glm::vec3(mesh.MeshMaterial.Ks.X, mesh.MeshMaterial.Ks.Y, mesh.MeshMaterial.Ks.Z);

 
    if (!mesh.MeshMaterial.map_Kd.empty())
    {
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrChannels;
        unsigned char* data = stbi_load(mesh.MeshMaterial.map_Kd.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glGenTextures(1, &m_TextureID);
            glBindTexture(GL_TEXTURE_2D, m_TextureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            GLenum format = GL_RGB;
            if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        stbi_image_free(data);
    }

	//VBO 
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(objl::Vertex), &mesh.Vertices[0], GL_STATIC_DRAW);

	//VBO 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOIDs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), &mesh.Indices[0], GL_STATIC_DRAW);
}


void Geometry::Draw()
{
	DrawGL();
}

void Geometry::AnimateRotate(bool start) // 회전을 위한 함수
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
		if (passed > 100) 
		{
			int animAmount = passed / 100;
			m_Y_RotationAngle -= animAmount * 0.1f; // 얼굴쪽으로 회전하도록 수정
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
    if (m_TextureID != 0)
    {
        glDeleteTextures(1, &m_TextureID);
    }
}
void Geometry::ChangeProgramGL(GLuint activeProgram)
{
	m_ProgramID = activeProgram;
	glBindVertexArray(m_VAOID);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOIDs[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOIDs[1]);

	// Vertex Attributes
	// Position
	GLuint posAttribLoc = glGetAttribLocation(m_ProgramID, "inPos");
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (GLvoid*)offsetof(objl::Vertex, Position));
	glEnableVertexAttribArray(posAttribLoc);

	// Normal
	GLuint normalAttribLoc = glGetAttribLocation(m_ProgramID, "inNormal");
	glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (GLvoid*)offsetof(objl::Vertex, Normal));
	glEnableVertexAttribArray(normalAttribLoc);

	// Texture Coordinate
	GLuint texAttribLoc = glGetAttribLocation(m_ProgramID, "inTexCoord");
	glVertexAttribPointer(texAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (GLvoid*)offsetof(objl::Vertex, TextureCoordinate));
	glEnableVertexAttribArray(texAttribLoc);
}
void Geometry::DrawGL()
{
	glUseProgram(m_ProgramID);

	GLuint useTextureLoc = glGetUniformLocation(m_ProgramID, "useTexture");

    if (m_TextureID != 0)
    {
		glUniform1i(useTextureLoc, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        GLuint texSamplerID = glGetUniformLocation(m_ProgramID, "textureSampler");
        glUniform1i(texSamplerID, 0);
    }
	else
	{
		glUniform1i(useTextureLoc, 0); 
	}

	GLuint wmatID = glGetUniformLocation(m_ProgramID, "wMat");
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), m_Y_RotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 finalWorld = m_World * rotationMatrix;
	glUniformMatrix4fv(wmatID, 1, GL_FALSE, glm::value_ptr(finalWorld));

	GLuint kdID = glGetUniformLocation(m_ProgramID, "Kd");
	glUniform3fv(kdID, 1, glm::value_ptr(m_Kd));

	glBindVertexArray(m_VAOID);
	glDrawElements(m_GLMode, m_NumIndices, GL_UNSIGNED_INT, 0);
}