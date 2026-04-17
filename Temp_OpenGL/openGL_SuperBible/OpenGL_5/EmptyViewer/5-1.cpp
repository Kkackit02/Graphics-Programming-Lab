#include<windows.h>
#include<GL/gl.h>
#include<GL/GLU.h>
#include<GL/glut.h>



//2-3 openGL, GLUT를 사용한 애니메이션
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 화면 초기화
	glShadeModel(GL_SMOOTH);

	glLoadIdentity(); // 매트릭스 초기화
	glTranslatef(0.0f, 0.0f, -600.0f); // 카메라에서 뒤로 삼각형을 보내기

	glBegin(GL_TRIANGLES);
	
	//적색
	glColor3ub((GLubyte)255, (GLubyte)0, (GLubyte)0);
	glVertex3f(0.0f, 200.0f, 0.0f);

	
	glColor3ub((GLubyte)0, (GLubyte)255, (GLubyte)0);
	glVertex3f(200.0f, -70.0f, 0.0f);


	glColor3ub((GLubyte)0, (GLubyte)0, (GLubyte)255);
	glVertex3f(-200.0f, -70.0f, 0.0f);
	glEnd();

	glFlush();

}


void SetupRC(void)
{
	glEnable(GL_DEPTH_TEST);              // 깊이 버퍼 사용
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);              // 배경 검정
}


//창 크기가 변할때 GLUT에 의해 호출
void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat fAspect;

	if (h == 0)
	{
		h = 1;
	}

	glViewport(0, 0, w, h); // 뷰포트 정의 (x,y, 너비, 높이) -> 현재 클리핑 영역을 해당 공간에 매핑
	fAspect = (GLfloat)w / (GLfloat)h;

	glMatrixMode(GL_PROJECTION); // gl 좌표계 초기화
	glLoadIdentity();

	gluPerspective(45.0f, fAspect, 1.0f, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	// sing buffer 창을 의미, 색상모드를 RGBA를 쓸것

	glutCreateWindow("Interpolation Triangle");

	glutDisplayFunc(RenderScene); // renderScene 함수를 출력콜백함수로 만들기
	glutReshapeFunc(ChangeSize);


	SetupRC(); //렌더링 전 초기화

	glutMainLoop();
}