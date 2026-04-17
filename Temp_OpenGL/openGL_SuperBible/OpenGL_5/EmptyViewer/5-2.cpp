#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

// 회전 각도
GLfloat xRot = 47.0f;
GLfloat yRot = 0.0f;

// 렌더링
void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -2.5f);  // 뒤로 이동
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f); // Y축 회전

    glColor3f(0.7f, 0.4f, 1.0f);
    glutSolidSphere(0.6f, 40, 40);  // 반지름 0.6, 세분화 40x40

    glPopMatrix();

    glutSwapBuffers();

    yRot += 0.5f;
    if (yRot >= 360.0f) yRot = 0.0f;
}

// 타이머
void TimerFunction(int value)
{
    glutPostRedisplay();
    glutTimerFunc(33, TimerFunction, 1);
}

// 초기화
void SetupRC()
{
    // 조명 설정
    GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat lightPos[] = { 1.0f, 1.0f, 1.0f, 0.0f };

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// 윈도우 리사이즈
void ChangeSize(GLsizei w, GLsizei h)
{
    if (h == 0) h = 1;
    GLfloat fAspect = (GLfloat)w / (GLfloat)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, fAspect, 1.0f, 425.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// main 함수
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Rotating Sphere");

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutTimerFunc(33, TimerFunction, 1);

    SetupRC();

    glutMainLoop();
    return 0;
}
