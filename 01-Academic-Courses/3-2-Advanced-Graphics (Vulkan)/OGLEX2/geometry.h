#pragma once

class Geometry
{
public:
	Geometry(GLuint activeProgram = 0);
	~Geometry();

	void CreateCube();
	void Draw();
	void SetPosition(glm::vec3 pos);
	void AnimateRotate(bool start = false);
public:
	glm::mat4 m_World;

	// OpenGL Related

	GLuint m_VAOID;
	GLuint m_VBOIDs[2];

protected:
	unsigned int m_NumVertices;
	std::chrono::high_resolution_clock::time_point m_AnimTime;

	// OpenGL Related
	GLuint m_ProgramID;
	GLuint m_GLMode;
	void InitGL();
	void DeleteGL();
	void ChangeProgramGL(GLuint activeProgram);
	void DrawGL();	
};
