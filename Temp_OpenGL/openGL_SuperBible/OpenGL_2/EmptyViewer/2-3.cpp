#include<windows.h>
#include<GL/gl.h>
#include<GL/GLU.h>
#include<GL/glut.h>

GLfloat x1 = 0.0f;
GLfloat y1 = 0.0f;
GLfloat rsize = 25;

//x,y 방향 이동속도
GLfloat xstep = 1.0f;
GLfloat ystep = 1.0f;

GLfloat windowWidth;
GLfloat windowHeight;




//2-3 openGL, GLUT를 사용한 애니메이션
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0f, 0.0f, 0.0f);

	glRectf(x1, y1, x1 + rsize, y1 - rsize);

	glutSwapBuffers(); // 버퍼를 교체하기(double buffer)
	//glFlush(); 필요없어짐!(버퍼 교체에서 간접적으로 실행되고있음)
}

void TimerFunction(int value)
{
	if (x1 > windowWidth - rsize || x1 < -windowWidth)
		xstep = -xstep;

	if (y1 > windowHeight || y1 < -windowHeight + rsize)
		ystep = -ystep;

	x1 += xstep;
	y1 += ystep;

	if (x1 > windowWidth - rsize + xstep)
		x1 = windowWidth - rsize - 1;
	else if (x1 < -(windowWidth + xstep))
		x1 = -windowWidth - 1;

	if (y1 > (windowHeight + ystep))
		y1 = windowHeight - 1;
	else if (y1 < -(windowHeight - rsize + ystep))
		y1 = -windowHeight + rsize - 1;

	//다시 그리기
	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 1);
}


void SetupRC(void)
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); //지우는 색을 청색으로 설정
}


//창 크기가 변할때 GLUT에 의해 호출
void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat aspectRatio = 1;

	if (h == 0)
	{
		h = 1;
	}

	glViewport(0, 0, w, h); // 뷰포트 정의 (x,y, 너비, 높이) -> 현재 클리핑 영역을 해당 공간에 매핑

	glMatrixMode(GL_PROJECTION); // gl 좌표계 초기화
	glLoadIdentity();
	
	// 클리핑 지정, l, r, b, t, n, f 순서
	 aspectRatio = (GLfloat)w / (GLfloat)h;


	 if (w <= h)
	 {
		 windowWidth = 100;
		 windowHeight = 100 / aspectRatio;
		 glOrtho(-100.0f, 100.0, -windowHeight , windowHeight, 1.0, -1.0);
	 }
	 else
	 {
		 windowWidth = 100 * aspectRatio;
		 windowHeight = 100;

		 glOrtho(-windowHeight, windowHeight, -100.0f, 100.0, 1.0, -1.0);
	 }

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	// sing buffer 창을 의미, 색상모드를 RGBA를 쓸것

	glutCreateWindow("Bounce");
	glutDisplayFunc(RenderScene); // renderScene 함수를 출력콜백함수로 만들기
	glutReshapeFunc(ChangeSize);

	glutTimerFunc(33, TimerFunction, 1);

	SetupRC(); //렌더링 전 초기화

	glutMainLoop();
}