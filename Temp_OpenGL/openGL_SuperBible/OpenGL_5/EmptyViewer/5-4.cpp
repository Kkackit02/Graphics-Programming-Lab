#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// 회전 각도
GLfloat xRot = 20.0f, yRot = 30.0f;

// 스포트라이트 위치와 방향
GLfloat spotPos[] = { 0.0f, 0.0f, 100.0f, 1.0f };
GLfloat spotDir[] = { 0.0f, 0.0f, -1.0f };

// 기본 광원
GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// 스포트라이트
GLfloat spotDiffuse[] = { 1.0f, 0.9f, 0.6f, 1.0f };

void SetupRC()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // 기본 광원 (GL_LIGHT0)
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    GLfloat defaultPos[] = { 0.0f, 50.0f, 100.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, defaultPos);

    // 스포트라이트 (GL_LIGHT1)
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, spotDiffuse);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 20.0f);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glClearColor(0, 0, 0, 1);
    glEnable(GL_NORMALIZE);
}

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    // 카메라 회전
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    // 스포트라이트 위치/방향 적용
    glLightfv(GL_LIGHT1, GL_POSITION, spotPos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);

    // 구 그리기
    glColor3ub(50, 120, 255);
    glutSolidSphere(30.0f, 50, 50);

    glPopMatrix();
    glutSwapBuffers();
}

void TimerFunction(int val)
{
    glutPostRedisplay();
    glutTimerFunc(16, TimerFunction, 1);
}

void ChangeSize(int w, int h)
{
    if (h == 0) h = 1;
    float ratio = (float)w / (float)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, ratio, 1.0, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 150, 0, 0, 0, 0, 1, 0);
}

void SpecialKeys(int key, int x, int y)
{
    // 방향키로 스포트라이트 위치 조정
    switch (key) {
    case GLUT_KEY_LEFT:
        spotPos[0] -= 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        spotPos[0] += 5.0f;
        break;
    case GLUT_KEY_UP:
        spotPos[1] += 5.0f;
        break;
    case GLUT_KEY_DOWN:
        spotPos[1] -= 5.0f;
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Sphere + Spotlight Control");

    SetupRC();

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutTimerFunc(16, TimerFunction, 1);

    glutMainLoop();
    return 0;
}
