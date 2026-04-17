#include<windows.h>
#include<GL/gl.h>
#include<GL/GLU.h>
#include<GL/glut.h>

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0f, 0.0f, 0.0f);

	glRectf(-25.0f, 25.0f, 25.0f, -25.0f);


	glFlush(); // 지금까지 받은 모든 명령어를 버퍼에서 비워내면서 모든 명령을 실행시키기
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


	//glOrtho의 작동 방식이 기존 클리핑 영역과 행렬을 곱한 값을 새로운 클리핑 영역으로 지정함 
	//따라서 상단에서 glMatrixMode로 초기화 필요
	//(Model -> camera -> perspective -> orthograph -> viewport)
	if (w <= h)
		glOrtho(-100.0f, 100.0, -100 / aspectRatio, 100.0 / aspectRatio, 1.0, -1.0);
	else
		glOrtho(-100.0f * aspectRatio, 100.0 * aspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// sing buffer 창을 의미, 색상모드를 RGBA를 쓸것

	glutCreateWindow("GLRECT");
	glutDisplayFunc(RenderScene); // renderScene 함수를 출력콜백함수로 만들기
	glutReshapeFunc(ChangeSize);
	SetupRC(); //렌더링 전 초기화

	glutMainLoop();
}