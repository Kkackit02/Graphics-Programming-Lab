#include<windows.h>
#include<GL/gl.h>
#include<GL/GLU.h>
#include<GL/glut.h>
#include <math.h>

#define GL_PI 3.1415f

GLfloat xRot = 120.0f;
GLfloat yRot = 30.0f;

//2-3 openGL, GLUT를 사용한 애니메이션
void RenderScene(void)
{
	GLfloat x, y, angle;
	GLboolean bCull = true;
	GLboolean bDepth = true;
	GLboolean bOutline = true;
	int iPivot = 1;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (bCull) //후면 추려내기 BACKFACE CULLING(물체 후면 없애기), 
		// 종이와 같이 편평한 물체나 유리같은 투명한 물체는 BackFace Culling을 적용해서는 안된다.
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if (bDepth) //z값에 따라 멀리있는건 앞에있는것에 덮어쒸워지게 설정
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if (bOutline)
		glPolygonMode(GL_BACK, GL_LINE);
	else
		glPolygonMode(GL_BACK, GL_FILL);

	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLE_FAN);

	glVertex3f(0.0f, 0.0f, 75.0f); // 공유 vertex

	for (angle = 0.0f; angle < (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		x = 50.0f * sin(angle);
		y = 50.0f * cos(angle);

		if ((iPivot % 2) == 0)
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.0f, 0.0f);

		iPivot++; //다음 삼각형에는 다른 색상 적용

		glVertex2f(x, y);
	}
	glEnd();
	
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.0f, 0.0f);
	for (angle = 0.0f; angle < (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		x = 50.0f * sin(angle);
		y = 50.0f * cos(angle);

		if ((iPivot % 2) == 0)
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.0f, 0.0f);

		iPivot++; //다음 삼각형에는 다른 색상 적용

		glVertex2f(x, y);
	}
	glEnd();
	
	glPopMatrix();
	glFlush();
}



void SetupRC(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glShadeModel(GL_FLAT);
	glFrontFace(GL_CCW); // 삼각형 팬일때는 시계 방향으로 와인딩을 해줘야 정면으로 보임
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