#include<windows.h>
#include<GL/gl.h>
#include<GL/GLU.h>
#include<GL/glut.h>
#include <math.h>

#define GL_PI 3.1415f

GLfloat xRot = 30.0f;
GLfloat yRot = 60.0f;

//2-3 openGL, GLUT를 사용한 애니메이션
void RenderScene(void)
{
	GLfloat x, y, z, angle;

	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	glBegin(GL_LINE_STRIP);
	z = -50.0f;
	for (angle = 0.0f; angle <= (2.0f * GL_PI) * 3.0f; angle += 0.1f)
	{
		x = 50.0f * sin(angle);
		y = 50.0f * cos(angle);

		glVertex3f(x, y, z);
		z += 0.5f;
	}
	glEnd();
	glPopMatrix();
	glFlush();
}



void SetupRC(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
}


//창 크기가 변할때 GLUT에 의해 호출
void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat aspectRatio = 1;

	if (h == 0)
	{
		h = 1;
	}

	GLfloat nRange = 100.0f;


	glViewport(0, 0, w, h); // 뷰포트 정의 (x,y, 너비, 높이) -> 현재 클리핑 영역을 해당 공간에 매핑

	glMatrixMode(GL_PROJECTION); // gl 좌표계 초기화
	glLoadIdentity();

	// 클리핑 지정, l, r, b, t, n, f 순서

	if (w <= h)
	{
		glOrtho(-nRange, nRange, -nRange * h / w, nRange * h / w, -nRange, nRange);
	}
	else
	{
		glOrtho(-nRange * h / w, nRange * h / w, -nRange, nRange, -nRange, nRange);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// sing buffer 창을 의미, 색상모드를 RGBA를 쓸것

	glutCreateWindow("vertex");
	glutDisplayFunc(RenderScene); // renderScene 함수를 출력콜백함수로 만들기
	glutReshapeFunc(ChangeSize);

	SetupRC(); //렌더링 전 초기화

	glutMainLoop();
}