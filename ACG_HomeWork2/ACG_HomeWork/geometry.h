#pragma once

#include "OBJ_Loader.h"

class Geometry
{
public:
	Geometry(GLuint activeProgram = 0);
	~Geometry();

	void InitFromMesh(const objl::Mesh& mesh);
	void Draw();
	void SetPosition(glm::vec3 pos);
	void AnimateRotate(bool start = false);
public:
	glm::mat4 m_World;

	// OpenGL Related

	GLuint m_VAOID;
	GLuint m_VBOIDs[2];
	GLuint m_TextureID;

protected:
	unsigned int m_NumIndices;
	std::chrono::high_resolution_clock::time_point m_AnimTime;

	// Material Properties
	glm::vec3 m_Ka;
	glm::vec3 m_Kd;
	glm::vec3 m_Ks;

	// OpenGL Related
	GLuint m_ProgramID;
	GLuint m_GLMode;
	void InitGL();
	void DeleteGL();
	void ChangeProgramGL(GLuint activeProgram);
	void DrawGL();	
};
