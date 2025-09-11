#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

// GLEW (Must be included before any other GL headers)
#include <GL/glew.h>

// FreeGLUT
#include <GL/freeglut.h>


using namespace std;

GLuint vbo; //vertex buffer object: 정점데이터를 GPU 메모리로 저장
GLuint vao; //vertex array Object : 정점 데이터의 속성을 저장
GLuint programID; //Vertex Shader와 Fragment Shader를 컴파일하고 링크한 셰이더 프로그램의 ID

vector<GLfloat> vertices; //vectice 동적 배열

void updateBuffer()
{
	//vertice가 변경될때마다 호출 ( 마우스 입력 받았을때마다 호출),
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	//바뀔때마다 GPU의 VBO로 최신 정점 데이터를 전송함
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if(VertexShaderStream.is_open())
	{
		string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	
	if (InfoLogLength > 0)
	{
		vector<char> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	}
	

	//Read the fragment shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if(FragmentShaderStream.is_open())
	{
		string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	
	if (InfoLogLength > 0)
	{
		vector<char> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	}
	

	//Link the program
	fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0)
	{
		vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	}
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}


void renderScene(void)
{
	//화면 초기화
	glClear(GL_COLOR_BUFFER_BIT); 
	
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

	//더블 버퍼링으로 깜빡임 방지(다음꺼 미리 그려놓기)
	glutSwapBuffers();
}


void init()
{
    //initilize the glew and check the errors.
    glewExperimental = true; // Needed for core profile
    GLenum res = glewInit();
    if(res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		getchar(); // pause to see error
    }

	glClearColor(1.0, 1.0, 1.0, 1.0); // 배경색 초기화
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE); 

	//Vao 생성
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// 정점 초기화(임의 값으로 초기화)
    vertices = {
        -0.1f, -0.9f, 0.0f,
        0.2f, -0.9f, 0.0f,
        -0.4f, -0.5f, 0.0f,

        0.9f,  0.9f, 0.0f,
		0.2f,  0.2f, 0.0f,
        0.7f,  0.5f, 0.0f,

		-0.9f, 0.9f, 0.0f,
		-0.5f, 0.3f, 0.0f,
		-0.1f, 0.5f, 0.0f,

		0.3f, -0.3f, 0.0f,
		1.0f, -0.3f, 0.0f,
		0.7f, -0.4f, 0.0f
    };

	//vbo 생성
	glGenBuffers(1, &vbo);

	
	updateBuffer(); //vertices 벡터들 GPU의 vbo로 복사

	programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
	// 셰이더 프로그램 생성 및 Program ID에 저장

	glUseProgram(programID);

	glEnableVertexAttribArray(0); // 0번 속성 -> 3개의 float로 이루어짐
	//VBO에 저장된 정점데이터와 셰이더의 layout을 어떻게 연결할지!


	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(
		0,                  // attribute 0.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?(정규화 유무)
		0,                  // stride
		(void*)0            // array buffer offset
	);
}

void mouse(int button, int state, int x, int y)
{
	//좌클릭 버튼 누르면 호출
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        float newX = (float)x / (480 / 2) - 1.0;
        float newY = 1.0 - (float)y / (480 / 2);
        
        vertices.push_back(newX);
        vertices.push_back(newY);
        vertices.push_back(0.0f);

        updateBuffer();
		//클릭한 좌표를 정점 벡터에 추가하고
		//UpdateBuffer() 호출하여 VBO에 최신화
        glutPostRedisplay();
		//화면 다시 그리기
    }
}


int main(int argc, char **argv)
{
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);

	//Specify OpenGL Version and Profile
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);

	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(480, 480);


	glutCreateWindow("ACG_Homework1_202112346");

	//call initization function
	init();

	glutDisplayFunc(renderScene); // 화면 초기화시 renderScene() 호출
	glutMouseFunc(mouse); // 마우스 버튼 이벤트시 mouse() 호출

	//enter GLUT event processing cycle
	glutMainLoop(); //입력 기다리면서 창 유지(이벤트 루프)

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(programID);
	
	return 1;
}