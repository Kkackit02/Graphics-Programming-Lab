
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

void MakeShadowMatrix(GLfloat ground[4], GLfloat lightPos[4], GLfloat shadowMat[16])
{
    GLfloat dot = ground[0] * lightPos[0] +
        ground[1] * lightPos[1] +
        ground[2] * lightPos[2] +
        ground[3] * lightPos[3];

    for (int i = 0; i < 16; ++i)
        shadowMat[i] = 0.0f;

    shadowMat[0] = dot - lightPos[0] * ground[0];
    shadowMat[4] = 0.0f - lightPos[0] * ground[1];
    shadowMat[8] = 0.0f - lightPos[0] * ground[2];
    shadowMat[12] = 0.0f - lightPos[0] * ground[3];

    shadowMat[1] = 0.0f - lightPos[1] * ground[0];
    shadowMat[5] = dot - lightPos[1] * ground[1];
    shadowMat[9] = 0.0f - lightPos[1] * ground[2];
    shadowMat[13] = 0.0f - lightPos[1] * ground[3];

    shadowMat[2] = 0.0f - lightPos[2] * ground[0];
    shadowMat[6] = 0.0f - lightPos[2] * ground[1];
    shadowMat[10] = dot - lightPos[2] * ground[2];
    shadowMat[14] = 0.0f - lightPos[2] * ground[3];

    shadowMat[3] = 0.0f - lightPos[3] * ground[0];
    shadowMat[7] = 0.0f - lightPos[3] * ground[1];
    shadowMat[11] = 0.0f - lightPos[3] * ground[2];
    shadowMat[15] = dot - lightPos[3] * ground[3];
}
void DrawGround(void)
{
    GLfloat fExtent = 20.0f;  // 바닥 범위
    GLfloat fStep = 1.0f;     // 한 타일 크기
    GLfloat y = -0.4f;

    int iCount = (int)(fExtent * 2 / fStep);  // 타일 개수

    glBegin(GL_QUADS);
    for (int i = -fExtent; i < fExtent; i += fStep) {
        for (int j = -fExtent; j < fExtent; j += fStep) {
            // 흰색과 회색을 번갈아가며 설정
            if (((int)(i + fExtent) + (int)(j + fExtent)) % 2 == 0)
                glColor3f(0.6f, 0.6f, 0.6f);  // 밝은 회색
            else
                glColor3f(0.3f, 0.3f, 0.3f);  // 어두운 회색

            // 각 타일 그리기 (사각형)
            glVertex3f(i, y, j);
            glVertex3f(i + fStep, y, j);
            glVertex3f(i + fStep, y, j + fStep);
            glVertex3f(i, y, j + fStep);
        }
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

    // 조명 위치
    GLfloat lightPos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // 땅 렌더링
    glDisable(GL_LIGHTING);
    DrawGround();
    glEnable(GL_LIGHTING);

    // 그림자 매트릭스 생성
    GLfloat shadowMat[16];
    GLfloat groundPlane[4] = { 0.0f, 1.0f, 0.0f, 0.4f }; // y = -0.4 평면
    MakeShadowMatrix(groundPlane, lightPos, shadowMat);

    // 객체 렌더링
    glColor3f(0.8f, 0.8f, 0.8f);
    for (int i = 0; i < NUM_SPHERES; i++) {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        glutSolidSphere(0.2f, 13, 26);
        glPopMatrix();
    }

    // 메인 객체 (회전 토러스) 렌더링
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

    // 그림자 렌더링
    glDisable(GL_LIGHTING);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glPushMatrix();
    glMultMatrixf(shadowMat);
    for (int i = 0; i < NUM_SPHERES; i++) {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        glutSolidSphere(0.2f, 13, 26);
        glPopMatrix();
    }
    glTranslatef(0.0f, 0.0f, -2.5f);
    glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(1.0f, 0.0f, 0.0f);
    glutSolidSphere(0.1f, 13, 26);
    glPopMatrix();
    glEnable(GL_LIGHTING);

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

    // 조명 설정
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

    // 재질 설정
    GLfloat matAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat matDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat matSpecular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat matShininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    // 구 위치 초기화
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
