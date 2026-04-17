#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define NUM_SPHERES 50
GLfloat yRot = 0.0f;

struct Frame {
    float pos[3];
    float forward[3];
    float up[3];

    Frame() {
        pos[0] = pos[1] = pos[2] = 0.0f;
        forward[0] = 0.0f; forward[1] = 0.0f; forward[2] = -1.0f;
        up[0] = 0.0f; up[1] = 1.0f; up[2] = 0.0f;
    }

    void ApplyCameraTransform() {
        float x[3], z[3], y[3];
        z[0] = -forward[0]; z[1] = -forward[1]; z[2] = -forward[2];
        x[0] = up[1] * z[2] - up[2] * z[1];
        x[1] = up[2] * z[0] - up[0] * z[2];
        x[2] = up[0] * z[1] - up[1] * z[0];
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

    glBegin(GL_QUADS);
    for (int i = -fExtent; i < fExtent; i += fStep) {
        for (int j = -fExtent; j < fExtent; j += fStep) {
            if (((int)(i + fExtent) + (int)(j + fExtent)) % 2 == 0)
                glColor4f(0.6f, 0.6f, 0.6f, 0.6f);
            else
                glColor4f(0.3f, 0.3f, 0.3f, 0.6f);

            glVertex3f(i, y, j);
            glVertex3f(i + fStep, y, j);
            glVertex3f(i + fStep, y, j + fStep);
            glVertex3f(i, y, j + fStep);
        }
    }
    glEnd();
}

void DrawWorld()
{
    for (int i = 0; i < NUM_SPHERES; i++) {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        glutSolidSphere(0.2f, 13, 26);
        glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(0.0f, 0.15f, -2.5f);

    glPushMatrix();
    glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(1.0f, 0.0f, 0.0f);
    glutSolidSphere(0.1f, 13, 26);
    glPopMatrix();

    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glutSolidTorus(0.15, 0.35, 20, 40);
    glPopMatrix();
}

void RenderScene(void)
{
    yRot += 0.5f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    frameCamera.ApplyCameraTransform();

    GLfloat lightPos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
    GLfloat lightPosMirror[] = { 0.0f, -5.8f, 5.0f, 1.0f };

    // [1] 반사된 월드
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosMirror);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glEnable(GL_LIGHTING);

    // 반사된 구들
    for (int i = 0; i < NUM_SPHERES; i++) {
        float y = spheres[i].pos[1];
        float reflectedY = -0.4f * 2 - y;  // y 대칭
        glPushMatrix();
        glTranslatef(spheres[i].pos[0], reflectedY + 0.0001f, spheres[i].pos[2]);
        glutSolidSphere(0.2f, 13, 26);
        glPopMatrix();
    }


    // 반사된 토러스 + 회전 구
    float baseY = 0.15f;
    float reflectY = 2 * -0.4f - baseY;
    glPushMatrix();
    glTranslatef(0.0f, reflectY + 0.0001f, -2.5f);
    glPushMatrix();
    glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(1.0f, 0.0f, 0.0f);
    glutSolidSphere(0.1f, 13, 26);
    glPopMatrix();
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    glDepthMask(GL_TRUE);
    glutSolidTorus(0.15, 0.35, 20, 40);

    glPopMatrix();

    glDisable(GL_BLEND);

    // [2] 반투명 바닥
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    DrawGround();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    // [3] 정방향 월드
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    DrawWorld();

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

	GLfloat flowLight[] = {0.1f, 0.1f, 0.1f, 1.0f};

    glClearColor(flowLight[0], flowLight[1], flowLight[2], flowLight[4]);
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, flowLight);
    glFogf(GL_FOG_START, 1.0f);
	glFogf(GL_FOG_END, 30.0f);
    glFogi(GL_FOG_MODE, GL_LINEAR);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat lightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    GLfloat matAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat matDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat matSpecular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat matShininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    //멀티 샘플링으로 안티에일리어싱!!
    glutInitWindowSize(600, 600);
    glutCreateWindow("Mirror Reflection - OpenGL");

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutTimerFunc(33, TimerFunction, 1);

    SetupRC();
    glutMainLoop();
    return 0;
}
