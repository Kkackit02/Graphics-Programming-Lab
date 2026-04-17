
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define NUM_SPHERES 50

// 간단한 프레임 구조체 정의
struct Frame {
    float pos[3];      // 위치
    float forward[3];  // 전방 벡터
    float up[3];       // 상단 벡터

    Frame() {
        pos[0] = pos[1] = pos[2] = 0.0f;
        forward[0] = 0.0f; forward[1] = 0.0f; forward[2] = -1.0f;
        up[0] = 0.0f; up[1] = 1.0f; up[2] = 0.0f;
    }

    void ApplyCameraTransform() {
        float x[3], z[3], y[3];
        // z = -forward
        z[0] = -forward[0]; z[1] = -forward[1]; z[2] = -forward[2];
        // x = up × z
        x[0] = up[1] * z[2] - up[2] * z[1];
        x[1] = up[2] * z[0] - up[0] * z[2];
        x[2] = up[0] * z[1] - up[1] * z[0];
        // y = z × x
        y[0] = z[1] * x[2] - z[2] * x[1];
        y[1] = z[2] * x[0] - z[0] * x[2];
        y[2] = z[0] * x[1] - z[1] * x[0];

        float mat[16] = {
            x[0], y[0], z[0], 0.0f,
            x[1], y[1], z[1], 0.0f,
            x[2], y[2], z[2], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(mat);
        glTranslatef(-pos[0], -pos[1], -pos[2]);
    }

    void MoveForward(float dist) {
        pos[0] += forward[0] * dist;
        pos[1] += forward[1] * dist;
        pos[2] += forward[2] * dist;
    }

    void RotateLocalY(float angle) {
        float cosA = cos(angle);
        float sinA = sin(angle);
        float newF[3] = {
            forward[0] * cosA + forward[2] * sinA,
            forward[1],
            -forward[0] * sinA + forward[2] * cosA
        };
        forward[0] = newF[0]; forward[1] = newF[1]; forward[2] = newF[2];
    }

    void SetOrigin(float x, float y, float z) {
        pos[0] = x; pos[1] = y; pos[2] = z;
    }

    void ApplyActorTransform() {
        glTranslatef(pos[0], pos[1], pos[2]);
    }
};

Frame spheres[NUM_SPHERES];
Frame frameCamera;

void DrawGround(void)
{
    GLfloat fExtent = 20.0f;
    GLfloat fStep = 1.0f;
    GLfloat y = -0.4f;
    GLint iLine;
    glBegin(GL_LINES);
    //격자무늬 땅 그리기
    for (iLine = -fExtent; iLine <= fExtent; iLine += fStep)
    {
        glVertex3f(iLine, y, fExtent);
        glVertex3f(iLine, y, -fExtent);
        glVertex3f(fExtent, y, iLine);
        glVertex3f(-fExtent, y, iLine);
    }
    glEnd();
}

void RenderScene(void)
{
    static GLfloat yRot = 0.0f;
    yRot += 0.5f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    frameCamera.ApplyCameraTransform();

    DrawGround();
    for (int i = 0; i < NUM_SPHERES; i++) {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        glutSolidSphere(0.2f, 13, 26);
        glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -2.5f);
    glPushMatrix();
    glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(1.0f, 0.0f, 0.0f);
    glutSolidSphere(0.1f, 13, 26);
    glPopMatrix();
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glutSolidTorus(0.15, 0.35, 20, 40);
    glPopMatrix();
    glPopMatrix();
    glutSwapBuffers();
}

void TimerFunction(int value)
{
    glutPostRedisplay();
    glutTimerFunc(33, TimerFunction, 1);
}

void SpecialKeys(int key, int x, int y)
{
    //키 입력받아서 카메라 프레임 조정
    if (key == GLUT_KEY_UP)
        frameCamera.MoveForward(0.1f);
    if (key == GLUT_KEY_DOWN)
        frameCamera.MoveForward(-0.1f);
    if (key == GLUT_KEY_LEFT)
        frameCamera.RotateLocalY(0.1f);
    if (key == GLUT_KEY_RIGHT)
        frameCamera.RotateLocalY(-0.1f);
    glutPostRedisplay();
}

void SetupRC()
{
    glClearColor(0.0f, 0.0f, .5f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    for (int i = 0; i < NUM_SPHERES; i++) {
        float x = ((float)((rand() % 400) - 200) * 0.1f);
        float z = ((float)((rand() % 400) - 200) * 0.1f);
        spheres[i].SetOrigin(x, 0.0f, z);
    }
}

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

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Frame-based World");

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutTimerFunc(33, TimerFunction, 1);

    SetupRC();

    glutMainLoop();
    return 0;
}
