#pragma once

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

#include "OBJ_Loader.h"
#include "stb_image.h"

class Geometry
{
public:
	// Member Variables
	glm::mat4 m_World;
	float m_Y_RotationAngle;
	float m_TranslateY;
	int m_TranslateDir;
	float m_Scale;
	int m_ScaleDir;
	GLuint m_VAOID;
	GLuint m_VBOIDs[2];
	GLuint m_TextureID;

protected:
	unsigned int m_NumIndices;
	std::chrono::high_resolution_clock::time_point m_AnimTime;
	glm::vec3 m_Ka;
	glm::vec3 m_Kd;
	glm::vec3 m_Ks;
	GLuint m_ProgramID;
	GLuint m_GLMode;

public:
	// Constructor
	Geometry(GLuint activeProgram = 0)
	{
		m_World = glm::mat4(1.0f);
		m_Y_RotationAngle = 0.0f;
		m_TranslateY = 0.0f;
		m_TranslateDir = 1;
		m_Scale = 1.0f;
		m_ScaleDir = 1;
		m_TextureID = 0;
		InitGL();
		ChangeProgramGL(activeProgram);
	}

	// Destructor
	~Geometry()
	{
		DeleteGL();
	}

	void SetPosition(glm::vec3 pos)
	{
		m_World = glm::translate(glm::mat4(1.0f), pos);
	}

	void InitFromMesh(const objl::Mesh& mesh)
	{
		m_GLMode = GL_TRIANGLES;
		m_NumIndices = static_cast<unsigned int>(mesh.Indices.size());

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
				if (nrChannels == 4) format = GL_RGBA;
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				stbi_image_free(data);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_VBOIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(objl::Vertex), &mesh.Vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOIDs[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), &mesh.Indices[0], GL_STATIC_DRAW);
	}

	void Draw()
	{
		DrawGL();
	}

	void AnimateRotate(bool start = false)
	{
		if (start == true) { m_AnimTime = std::chrono::high_resolution_clock::now(); }
		else
		{
			auto curTime = std::chrono::high_resolution_clock::now();
			auto passedMS = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - m_AnimTime);
			long passed = static_cast<long>(passedMS.count());
			if (passed > 100)
			{
				int animAmount = passed / 100;
				m_Y_RotationAngle -= animAmount * 0.1f;
				m_AnimTime = curTime;
			}
		}
	}

	void AnimateTranslate(bool start = false)
	{
		if (m_TranslateDir == 1) {
			m_TranslateY += 0.05f;
			if (m_TranslateY > 1.0f) m_TranslateDir = -1;
		}
		else {
			m_TranslateY -= 0.05f;
			if (m_TranslateY < -1.0f) m_TranslateDir = 1;
		}
	}

	void AnimateScale(bool start = false)
	{
		if (m_ScaleDir == 1) {
			m_Scale += 0.01f;
			if (m_Scale > 1.5f) m_ScaleDir = -1;
		}
		else {
			m_Scale -= 0.01f;
			if (m_Scale < 0.5f) m_ScaleDir = 1;
		}
	}

protected:
	void InitGL()
	{
		glGenVertexArrays(1, &m_VAOID);
		glGenBuffers(2, m_VBOIDs);
	}

	void DeleteGL()
	{
		glDeleteVertexArrays(1, &m_VAOID);
		glDeleteBuffers(2, m_VBOIDs);
		if (m_TextureID != 0)
		{
			glDeleteTextures(1, &m_TextureID);
		}
	}

	void ChangeProgramGL(GLuint activeProgram)
	{
		m_ProgramID = activeProgram;
		glBindVertexArray(m_VAOID);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOIDs[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOIDs[1]);
		GLuint posAttribLoc = glGetAttribLocation(m_ProgramID, "inPos");
		glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (GLvoid*)offsetof(objl::Vertex, Position));
		glEnableVertexAttribArray(posAttribLoc);
		GLuint normalAttribLoc = glGetAttribLocation(m_ProgramID, "inNormal");
		glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (GLvoid*)offsetof(objl::Vertex, Normal));
		glEnableVertexAttribArray(normalAttribLoc);
		GLuint texAttribLoc = glGetAttribLocation(m_ProgramID, "inTexCoord");
		glVertexAttribPointer(texAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (GLvoid*)offsetof(objl::Vertex, TextureCoordinate));
		glEnableVertexAttribArray(texAttribLoc);
	}

	void DrawGL()
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
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(m_Scale));
		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, m_TranslateY, 0.0f));
		glm::mat4 finalWorld = m_World * translateMatrix * rotationMatrix * scaleMatrix;
		glUniformMatrix4fv(wmatID, 1, GL_FALSE, glm::value_ptr(finalWorld));
		GLuint kdID = glGetUniformLocation(m_ProgramID, "Kd");
		glUniform3fv(kdID, 1, glm::value_ptr(m_Kd));
		glBindVertexArray(m_VAOID);
		glDrawElements(m_GLMode, m_NumIndices, GL_UNSIGNED_INT, 0);
	}
};