/********************************************************************/
/*         GLU Quadric Demo Viewer (sphere / tex-sphere / etc.)     */
/*   필요 헤더: windows.h, gl.h, glu.h, glut.h, math.h, stdlib.h    */
/********************************************************************/
#define _USE_MATH_DEFINES
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* ---------- 전역 ---------- */
GLuint  gEarthTex = 0;
bool    gSpinning = true;
float   gAngle = 0.0f;
int     gDemoIdx = 0;                // 0~5
const   int DEMO_COUNT = 6;

/* ---------- 간단 구텍스처(절차적 위도-경도 줄무늬) ---------- */
static void MakeStripeTex(GLubyte img[256][256][3])
{
    for (int y = 0; y < 256; ++y)
        for (int x = 0; x < 256; ++x) {
            bool land = ((x / 8) % 2) == 0;     // 세로 줄무늬
            GLubyte c = land ? 200 : 120;
            img[y][x][0] = c; img[y][x][1] = land ? 160 : 80; img[y][x][2] = 60;
        }
}
static void CreateEarthTexture()
{
    GLubyte img[256][256][3];
    MakeStripeTex(img);
    glGenTextures(1, &gEarthTex);
    glBindTexture(GL_TEXTURE_2D, gEarthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

/* ---------- 각 도형 ---------- */
void DrawSolidSphere(double r)
{
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluSphere(q, r, 32, 32);
    gluDeleteQuadric(q);
}
void DrawTexturedSphere(double r)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gEarthTex);
    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    gluSphere(q, r, 36, 36);
    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}
void DrawClosedCylinder(float baseR, float topR, float h)
{
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, baseR, topR, h, 32, 4);

    // 바닥
    glPushMatrix(); glRotatef(180, 1, 0, 0); gluDisk(q, 0, baseR, 32, 1); glPopMatrix();
    // 윗면
    glPushMatrix(); glTranslatef(0, 0, h);  gluDisk(q, 0, topR, 32, 1); glPopMatrix();
    gluDeleteQuadric(q);
}
void DrawHalfDisk(float inR, float outR)
{
    GLUquadric* q = gluNewQuadric();
    gluPartialDisk(q, inR, outR, 48, 1, 0, 180);
    gluDeleteQuadric(q);
}
void DrawTrumpet(float r0, float r1, float h)
{
    GLUquadric* body = gluNewQuadric();
    gluCylinder(body, r0, r1, h, 30, 4);
    gluDeleteQuadric(body);
    GLUquadric* rim = gluNewQuadric();
    gluQuadricDrawStyle(rim, GLU_LINE);
    glPushMatrix(); glTranslatef(0, 0, h);
    gluDisk(rim, r1 * 0.9f, r1, 30, 1);
    glPopMatrix();
    gluDeleteQuadric(rim);
}
void DrawBreathingTube(float r, float h, float phase)
{
    float rNow = r * (1.f + 0.05f * sinf(phase));
    GLUquadric* tube = gluNewQuadric();
    gluCylinder(tube, rNow, rNow, h, 40, 8);
    gluDeleteQuadric(tube);
}

/* ---------- 렌더 ---------- */
void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    gluLookAt(0, 0, 6, 0, 0, 0, 0, 1, 0);

    glRotatef(gAngle, 0, 1, 0);

    switch (gDemoIdx)
    {
    case 0: DrawSolidSphere(1.5);                    break;
    case 1: DrawTexturedSphere(1.5);                 break;
    case 2: DrawClosedCylinder(1, 0.8, 2.5);           break;
    case 3: DrawHalfDisk(0.5, 1.5);                   break;
    case 4: DrawTrumpet(0.4, 1.2, 2);                  break;
    case 5: DrawBreathingTube(0.6, 2.5, gAngle * M_PI / 180); break;
    }

    glutSwapBuffers();
}

/* ---------- 콜백 ---------- */
void Idle()
{
    if (gSpinning) { gAngle += 0.01f; if (gAngle > 360)gAngle -= 360; }
    glutPostRedisplay();
}
void Special(int key, int, int)
{
    if (key == GLUT_KEY_RIGHT) { gDemoIdx = (gDemoIdx + 1) % DEMO_COUNT; }
    if (key == GLUT_KEY_LEFT) { gDemoIdx = (gDemoIdx + DEMO_COUNT - 1) % DEMO_COUNT; }
}
void Key(unsigned char k, int, int)
{
    if (k == ' ') { gSpinning = !gSpinning; }
}

/* ---------- 초기화 ---------- */
void SetupRC()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    GLfloat lPos[] = { 0,3,3,1 }, lAmb[] = { 0.2,0.2,0.2,1 }, lDif[] = { 0.8,0.8,0.8,1 };
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lDif);

    GLfloat matSpec[] = { 1,1,1,1 }, shin[] = { 64 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, shin);

    CreateEarthTexture();
}
void Reshape(int w, int h)
{
    if (!h)h = 1;
    float asp = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(45, asp, 1, 100);
}

/* ---------- main ---------- */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("GLU Quadric Viewer (←/→ switch, SPACE pause)");

    SetupRC();

    glutDisplayFunc(RenderScene);
    glutIdleFunc(Idle);
    glutReshapeFunc(Reshape);
    glutSpecialFunc(Special);
    glutKeyboardFunc(Key);

    glutMainLoop();
    return 0;
}
