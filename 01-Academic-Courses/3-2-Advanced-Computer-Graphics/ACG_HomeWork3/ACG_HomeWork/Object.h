#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "OBJ_Loader.h"
#include "stb_image.h"

class Object
{
public:
    glm::vec3 m_Position; 
    float m_Y_RotationAngle;
    float m_TranslateY;
    int m_TranslateDir;
    float m_Scale;
    int m_ScaleDir;

protected:
    GLuint m_VAOID;
    GLuint m_VBOIDs[2];
    GLuint m_TextureID;
    unsigned int m_NumIndices;
    
    glm::vec3 m_Ka;
    glm::vec3 m_Kd;
    glm::vec3 m_Ks;

    GLuint m_ProgramID;

public: 
    Object(GLuint activeProgram)
    {
        m_ProgramID = activeProgram;
        m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
        m_Y_RotationAngle = 0.0f;
        m_TranslateY = 0.0f;
        m_TranslateDir = 1;
        m_Scale = 1.0f;
        m_ScaleDir = 1;
        m_TextureID = 0;
        m_NumIndices = 0;
        m_Ka = glm::vec3(0.0f);
        m_Kd = glm::vec3(0.0f);
        m_Ks = glm::vec3(0.0f);

        glGenVertexArrays(1, &m_VAOID);
        glGenBuffers(2, m_VBOIDs);
    }

    ~Object()
    {
        glDeleteVertexArrays(1, &m_VAOID);
        glDeleteBuffers(2, m_VBOIDs);
        if (m_TextureID != 0)
        {
            glDeleteTextures(1, &m_TextureID);
        }
    }

    void SetPosition(glm::vec3 pos)
    {
        m_Position = pos;
    }

    void InitFromMesh(const objl::Mesh& mesh)
    {
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
                GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
				stbi_image_free(data); 
            }
        }

        glBindVertexArray(m_VAOID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBOIDs[0]);
        glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(objl::Vertex), &mesh.Vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOIDs[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), &mesh.Indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (GLvoid*)offsetof(objl::Vertex, Position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (GLvoid*)offsetof(objl::Vertex, Normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (GLvoid*)offsetof(objl::Vertex, TextureCoordinate));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }

    void Draw(const glm::mat4& view, const glm::mat4& projection)
    {
   

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

  
        glm::mat4 worldMatrix = glm::mat4(1.0f);
        worldMatrix[3][0] = m_Position.x;
        worldMatrix[3][1] = m_Position.y;
        worldMatrix[3][2] = m_Position.z;
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), m_Y_RotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(m_Scale));
        glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, m_TranslateY, 0.0f));
        glm::mat4 finalWorld = worldMatrix * translateMatrix * rotationMatrix * scaleMatrix;

        glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, "wMat"), 1, GL_FALSE, glm::value_ptr(finalWorld));

        glUniform3fv(glGetUniformLocation(m_ProgramID, "Kd"), 1, glm::value_ptr(m_Kd));

        glBindVertexArray(m_VAOID);
        glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void AnimateRotate_L()
    {
        m_Y_RotationAngle -= 0.01f;
    }

    void AnimateTranslate()
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
};