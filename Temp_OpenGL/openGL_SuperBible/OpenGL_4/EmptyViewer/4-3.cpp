#include<windows.h>
#include<GL/gl.h>
#include<GL/GLU.h>
#include<GL/glut.h>

GLfloat rsize = 25;
//x,y 방향 이동속도
GLfloat lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };  // 태양 위치 (월드 좌표)

GLfloat windowWidth;
GLfloat windowHeight;




//2-3 openGL, GLUT를 사용한 애니메이션
void RenderScene(void)
{
	// Earth and moon angle of revolution
	static float fMoonRot = 0.0f;
	static float fEarthRot = 0.0f;
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Save the matrix state and do the rotations
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	// Translate the whole scene out and into view
	glTranslatef(0.0f, 0.0f, -300.0f);
	// Set material color, to yellow
	// Sun
	glColor3ub(255, 255, 0);
	glDisable(GL_LIGHTING);
	glutSolidSphere(15.0f, 15, 15);
	glEnable(GL_LIGHTING);
	// Position the light after we draw the Sun!
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	// Rotate coordinate system
	glRotatef(fEarthRot, 0.0f, 1.0f, 0.0f);
	// Draw the earth
	glColor3ub(0, 0, 255);
	glTranslatef(105.0f, 0.0f, 0.0f);
	glutSolidSphere(15.0f, 15, 15);
	// Rotate from Earth-based coordinates and draw moon
	glColor3ub(200, 200, 200);
	glRotatef(fMoonRot, 0.0f, 1.0f, 0.0f);
	glTranslatef(30.0f, 0.0f, 0.0f);
	fMoonRot += 15.0f;
	if (fMoonRot > 360.0f)
		fMoonRot = 0.0f;
	glutSolidSphere(6.0f, 15, 15);
	// Restore the matrix state
	glPopMatrix(); // Modelview matrix
	// Step Earth orbit 5 degrees
	fEarthRot += 5.0f;
	if (fEarthRot > 360.0f)
		fEarthRot = 0.0f;
	glutSwapBuffers();
}


void TimerFunction(int value)
{
	//다시 그리기
	glutPostRedisplay();
	glutTimerFunc(50, TimerFunction, 1);
}


void SetupRC(void)
{
	glEnable(GL_DEPTH_TEST);              // 깊이 버퍼 사용
	glEnable(GL_LIGHTING);               // 조명 사용
	glEnable(GL_LIGHT0);                 // 0번 광원 활성화
	glEnable(GL_COLOR_MATERIAL);         // 색상 적용 활성화

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // 색상 속성 설정
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);              // 배경 검정glEnable(GL_DEPTH_TEST);              // 깊이 버퍼 사용
    glEnable(GL_LIGHTING);               // 조명 사용
    glEnable(GL_LIGHT0);                 // 0번 광원 활성화
    glEnable(GL_COLOR_MATERIAL);         // 색상 적용 활성화

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // 색상 속성 설정
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

	gluPerspective(45.0f, fAspect, 1.0f, 425.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	// sing buffer 창을 의미, 색상모드를 RGBA를 쓸것

	glutCreateWindow("Solar");
	glutDisplayFunc(RenderScene); // renderScene 함수를 출력콜백함수로 만들기
	glutReshapeFunc(ChangeSize);

	glutTimerFunc(33, TimerFunction, 1);

	SetupRC(); //렌더링 전 초기화

	glutMainLoop();
}