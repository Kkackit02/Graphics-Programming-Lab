#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

class Spline
{
public:
    enum CurveType {
        HERMITE_CATMULL_ROM = 0,
        BEZIER = 1,
        B_SPLINE = 2
    };

    Spline(const std::vector<glm::vec3>& controlPoints)
        : m_currentCurveType(HERMITE_CATMULL_ROM)
    {
        m_spline_programID = LoadShaders("Spline_VertexShader.txt", "Spline_FragmentShader.txt", "Spline_GeometryShader.txt");
        m_controlPoints = controlPoints;

        // Hermite
        m_paddedPoints = controlPoints;
        if (m_paddedPoints.size() > 1) {
            m_paddedPoints.insert(m_paddedPoints.begin(), m_paddedPoints.front());
            m_paddedPoints.push_back(m_paddedPoints.back());
        }
        m_numPaddedPoints = static_cast<int>(m_paddedPoints.size());

        glGenVertexArrays(1, &m_hermite_vaoID);
        glBindVertexArray(m_hermite_vaoID);
        glGenBuffers(1, &m_hermite_vboID);
        glBindBuffer(GL_ARRAY_BUFFER, m_hermite_vboID);
        glBufferData(GL_ARRAY_BUFFER, m_paddedPoints.size() * sizeof(glm::vec3), m_paddedPoints.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Bazier
        glGenVertexArrays(1, &m_bezier_vaoID);
        glBindVertexArray(m_bezier_vaoID);
        glGenBuffers(1, &m_bezier_vboID);
        glBindBuffer(GL_ARRAY_BUFFER, m_bezier_vboID);
        glBufferData(GL_ARRAY_BUFFER, m_controlPoints.size() * sizeof(glm::vec3), m_controlPoints.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // ControlPoint
        m_points_programID = LoadShaders("Point_VertexShader.txt", "Point_FragmentShader.txt");
        glGenVertexArrays(1, &m_points_vaoID);
        glBindVertexArray(m_points_vaoID);
        glGenBuffers(1, &m_points_vboID);
        glBindBuffer(GL_ARRAY_BUFFER, m_points_vboID);
        glBufferData(GL_ARRAY_BUFFER, m_controlPoints.size() * sizeof(glm::vec3), m_controlPoints.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    ~Spline()
    {
        glDeleteProgram(m_spline_programID);
        glDeleteVertexArrays(1, &m_hermite_vaoID);
        glDeleteBuffers(1, &m_hermite_vboID);
        glDeleteVertexArrays(1, &m_bezier_vaoID);
        glDeleteBuffers(1, &m_bezier_vboID);

        glDeleteProgram(m_points_programID);
        glDeleteVertexArrays(1, &m_points_vaoID);
        glDeleteBuffers(1, &m_points_vboID);
    }

    void SetCurveType(CurveType type)
    {
        m_currentCurveType = type;
    }

    glm::vec3 getPointOnSpline(float t) //spline의 좌표 반환 함수(애니메이션용)
    {
        if (m_controlPoints.size() < 4) {// 점 개수 예외처리
            if (m_controlPoints.size() < 2)
            {

                return glm::vec3(0.0f);
            }

            float numSegments = static_cast<float>(m_controlPoints.size() - 1);
            float segment_t = t * numSegments;
            int segment_index = static_cast<int>(floor(segment_t));
            if (segment_index >= numSegments) segment_index = static_cast<int>(numSegments - 1);
            float local_t = segment_t - segment_index;

            return glm::mix(m_controlPoints[segment_index], m_controlPoints[segment_index + 1], local_t);
        }

        if (m_currentCurveType == BEZIER)
        {
            int numSegments = m_controlPoints.size() / 4;
            if (numSegments == 0)
            {
                return m_controlPoints[0];
            }

            float segment_t = t * numSegments;
            int segment_index = static_cast<int>(floor(segment_t));
            if (segment_index >= numSegments)
            {
                segment_index = numSegments - 1;
            }
            float local_t = segment_t - segment_index;

            int p_idx = segment_index * 4;
            glm::vec3 p0 = m_controlPoints[p_idx];
            glm::vec3 p1 = m_controlPoints[p_idx + 1];
            glm::vec3 p2 = m_controlPoints[p_idx + 2];
            glm::vec3 p3 = m_controlPoints[p_idx + 3];

            float one_minus_t = 1.0f - local_t;
            return pow(one_minus_t, 3.0f) * p0 +
                   3.0f * local_t * pow(one_minus_t, 2.0f) * p1 +
                   3.0f * pow(local_t, 2.0f) * one_minus_t * p2 +
                   pow(local_t, 3.0f) * p3;
        }
        else // B-Spline, Hermite/Catmull-Rom
        {
            float numSegments = static_cast<float>(m_controlPoints.size() - 1);
            float segment_t = t * numSegments;
            int segment_index = static_cast<int>(floor(segment_t));
            if (segment_index >= numSegments)
            {
                segment_index = static_cast<int>(numSegments - 1);
            }
            float local_t = segment_t - segment_index;

            glm::vec3 p0 = m_paddedPoints[segment_index];
            glm::vec3 p1 = m_paddedPoints[segment_index + 1];
            glm::vec3 p2 = m_paddedPoints[segment_index + 2];
            glm::vec3 p3 = m_paddedPoints[segment_index + 3];

            float lt2 = local_t * local_t;
            float lt3 = lt2 * local_t;

            if (m_currentCurveType == B_SPLINE)
            {
                float b0 = (1.0f/6.0f) * (-lt3 + 3.0f*lt2 - 3.0f*local_t + 1.0f);
                float b1 = (1.0f/6.0f) * (3.0f*lt3 - 6.0f*lt2 + 4.0f);
                float b2 = (1.0f/6.0f) * (-3.0f*lt3 + 3.0f*lt2 + 3.0f*local_t + 1.0f);
                float b3 = (1.0f/6.0f) * (lt3);
                return b0 * p0 + b1 * p1 + b2 * p2 + b3 * p3;
            }
            else // Default to HERMITE_CATMULL_ROM
            {
                return 0.5f * ( (2.0f * p1) + (-p0 + p2) * local_t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * lt2 + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * lt3 );
            }
        }
    }

    void Draw(const glm::mat4& view, const glm::mat4& projection) 
    {
        glUseProgram(m_spline_programID);
        glUniformMatrix4fv(glGetUniformLocation(m_spline_programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(m_spline_programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform1i(glGetUniformLocation(m_spline_programID, "curveType"), static_cast<int>(m_currentCurveType));

        if (m_currentCurveType == BEZIER)
        {
            if (m_controlPoints.size() >= 4) {
                int num_bezier_points = (m_controlPoints.size() / 4) * 4;
                glBindVertexArray(m_bezier_vaoID);
                glDrawArrays(GL_LINES_ADJACENCY, 0, num_bezier_points);
            }
        }
        else 
        {
            if (m_controlPoints.size() >= 2) {
                glBindVertexArray(m_hermite_vaoID);
                glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, m_numPaddedPoints);
            }
        }
        //control point
        glUseProgram(m_points_programID);
        glUniformMatrix4fv(glGetUniformLocation(m_points_programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(m_points_programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glEnable(GL_PROGRAM_POINT_SIZE);
        glBindVertexArray(m_points_vaoID);
        glDrawArrays(GL_POINTS, 0, m_controlPoints.size());
        glDisable(GL_PROGRAM_POINT_SIZE);

        glBindVertexArray(0);
    }

private:
    GLuint m_spline_programID;
    CurveType m_currentCurveType;

    // Hermite
    GLuint m_hermite_vaoID;
    GLuint m_hermite_vboID;
    int m_numPaddedPoints;
    std::vector<glm::vec3> m_paddedPoints;

    // Bezier
    GLuint m_bezier_vaoID;
    GLuint m_bezier_vboID;

    // control points
    GLuint m_points_programID;
    GLuint m_points_vaoID;
    GLuint m_points_vboID;
    std::vector<glm::vec3> m_controlPoints;

    GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path = nullptr)
    {
        auto loadSub = [](GLuint shader_type, const char* file_path) -> GLuint
        {
            GLuint shaderID = glCreateShader(shader_type);
            std::string ShaderCode;
            std::ifstream ShaderStream(file_path, std::ios::in);
            if (ShaderStream.is_open()) {
                std::string Line = "";
                while (getline(ShaderStream, Line))
                    ShaderCode += "\n" + Line;
                ShaderStream.close();
            } else {
                printf("Cannot open %s\n", file_path);
                return 0;
            }
            GLint Result = GL_FALSE;
            int InfoLogLength;
            printf("Compiling shader : %s\n", file_path);
            char const* SourcePointer = ShaderCode.c_str();
            glShaderSource(shaderID, 1, &SourcePointer, NULL);
            glCompileShader(shaderID);
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
            if (InfoLogLength > 0) {
                std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
                glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
                printf("%s\n", &ShaderErrorMessage[0]);
            }
            return shaderID;
        };

        GLuint VertexShaderID = loadSub(GL_VERTEX_SHADER, vertex_file_path);
        GLuint FragmentShaderID = loadSub(GL_FRAGMENT_SHADER, fragment_file_path);
        GLuint GeometryShaderID = 0;
        if (geometry_file_path != nullptr) {
            GeometryShaderID = loadSub(GL_GEOMETRY_SHADER, geometry_file_path);
        }

        printf("Linking program (%s, %s)\n", vertex_file_path, fragment_file_path);
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        if (GeometryShaderID != 0) {
            glAttachShader(ProgramID, GeometryShaderID);
        }
        glLinkProgram(ProgramID);

        GLint Result = GL_FALSE;
        int InfoLogLength;
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }

        glDetachShader(ProgramID, VertexShaderID);
        glDetachShader(ProgramID, FragmentShaderID);
        if (GeometryShaderID != 0) {
            glDetachShader(ProgramID, GeometryShaderID);
        }

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
        if (GeometryShaderID != 0) {
            glDeleteShader(GeometryShaderID);
        }

        return ProgramID;
    }

    GLuint LoadSplineShaders(const char* v, const char* f, const char* g) { return LoadShaders(v, f, g); }
    GLuint LoadPointShaders(const char* v, const char* f) { return LoadShaders(v, f, nullptr); }
};