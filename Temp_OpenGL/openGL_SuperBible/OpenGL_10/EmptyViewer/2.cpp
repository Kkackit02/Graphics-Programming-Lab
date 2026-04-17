/********************************************************************/
/* Snowman with GLU Quadrics (based on SuperBible Figure 10-9)      */
/* 빌드 예시(MinGW)                                                 */
/*   g++ snowman.cpp -o snowman.exe -lopengl32 -lglu32 -lfreeglut   */
/********************************************************************/
#define _USE_MATH_DEFINES
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

/* -------- 전역 -------- */
GLfloat xRot = 20.0f;      // 상하 회전
GLfloat yRot = 30.0f;      // 좌우 회전
GLuint gSnowTex = 0;    // 눈바닥·몸통에 쓸 텍스처
/* ── 눈 덮인 바닥 ── */
void DrawSnowGround()
{
    const float SIZE = 40.0f;   // ±20 범위
    const float TILE = 10.0f;   // 텍스처 반복 크기
    const float Y = -1.4f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gSnowTex);

    glColor3f(1, 1, 1);
    glNormal3f(0, 1, 0);          // 윗면 노멀

    glBegin(GL_QUADS);
    /* CCW(반시계) 순서로 배치 */
    glTexCoord2f(0, 0);   glVertex3f(-SIZE, Y, -SIZE);
    glTexCoord2f(0, TILE);   glVertex3f(-SIZE, Y, SIZE);
    glTexCoord2f(TILE, TILE);  glVertex3f(SIZE, Y, SIZE);
    glTexCoord2f(TILE, 0);     glVertex3f(SIZE, Y, -SIZE);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}






void RenderScene()
{
    GLUquadricObj* pObj;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    DrawSnowGround();                                // ★ 바닥 먼저


    glPushMatrix();
    /* 카메라 위치 조정 */
    glTranslatef(0.0f, -0.8f, -4.0f);
    glRotatef(xRot, 1, 0, 0);
    glRotatef(yRot, 0, 1, 0);

    /* Quadric 생성 */
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);

    /* ───── 눈사람 몸통 ───── */
    glPushMatrix();
    glColor3f(1, 1, 1);
    gluSphere(pObj, 0.40, 26, 13);          // 아래 몸통

    glTranslatef(0, 0.55, 0);
    gluSphere(pObj, 0.30, 26, 13);          // 중간

    glTranslatef(0, 0.45, 0);
    gluSphere(pObj, 0.24, 26, 13);          // 머리

    /* 눈 */
    glColor3f(0, 0, 0);
    glTranslatef(0.10f, 0.10f, 0.21f);
    gluSphere(pObj, 0.02, 26, 13);
    glTranslatef(-0.20f, 0.0f, 0.0f);
    gluSphere(pObj, 0.02, 26, 13);

    /* 코(당근) */
    glColor3f(1.0f, 0.3f, 0.3f);
    glTranslatef(0.10f, -0.12f, 0.0f);
    gluCylinder(pObj, 0.04, 0.0, 0.30, 26, 13);
    glPopMatrix();

    /* ───── 모자 ───── */
    glPushMatrix();
    glColor3f(0, 0, 0);
    glTranslatef(0.0f, 1.17f, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);          // 원통을 위로

    gluCylinder(pObj, 0.17f, 0.17f, 0.40f, 26, 13); // 본체

    /* 챙(브림) : 앞뒤 안보이도록 컬링 끄고 두 장의 Disk */
    glDisable(GL_CULL_FACE);
    gluDisk(pObj, 0.17f, 0.28f, 26, 13);            // 아랫면 챙
    glEnable(GL_CULL_FACE);

    glTranslatef(0, 0, 0.40f);                        // 끝단
    gluDisk(pObj, 0.0f, 0.17f, 26, 13);             // 윗뚜껑
    glPopMatrix();

    gluDeleteQuadric(pObj);
    glPopMatrix();

    glutSwapBuffers();
}

/* -------- 키보드 : 방향키로 눈사람 돌리기 -------- */
void SpecialKeys(int key, int, int)
{
    if (key == GLUT_KEY_UP)    xRot -= 5.0f;
    if (key == GLUT_KEY_DOWN)  xRot += 5.0f;
    if (key == GLUT_KEY_LEFT)  yRot -= 5.0f;
    if (key == GLUT_KEY_RIGHT) yRot += 5.0f;

    if (xRot > 356.0f) xRot -= 360.0f;
    if (xRot < -1.0f)  xRot += 360.0f;
    if (yRot > 356.0f) yRot -= 360.0f;
    if (yRot < -1.0f)  yRot += 360.0f;

    glutPostRedisplay();
}
/* --- 눈 텍스처 데이터 생성 --- */
static void MakeSnowTexture(GLubyte img[256][256][3])
{
    for (int y = 0; y < 256; ++y)
        for (int x = 0; x < 256; ++x) {
            GLubyte shade = 230 + rand() % 26;       // 밝은 회색 노이즈
            img[y][x][0] = img[y][x][1] = img[y][x][2] = shade;
        }
}

/* --- 텍스처 객체 생성 & 업로드 --- */
static void CreateSnowTex()
{
    GLubyte img[256][256][3];
    MakeSnowTexture(img);

    glGenTextures(1, &gSnowTex);
    glBindTexture(GL_TEXTURE_2D, gSnowTex);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB,
        256, 256, GL_RGB, GL_UNSIGNED_BYTE, img);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}



/* -------- 초기화 -------- */
void SetupRC()
{
    glClearColor(0.5f, 0.6f, 0.8f, 1.0f);  // 연한 하늘색
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    /* 약한 조명 */
    GLfloat  white[4] = { 1,1,1,1 };
    GLfloat  amb[4] = { 0.2f,0.2f,0.2f,1 };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    GLfloat lightPos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_COLOR_MATERIAL);                         // 추가
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);   // 추가

    /* ── 안개 설정 ── */                         // ★
    GLfloat fogColor[4] = { 0.6f,0.75f,0.9f,1 };   // 배경과 같은 톤
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 4.0f);   // 카메라에서 12유닛 뒤부터
    glFogf(GL_FOG_END, 10.0f);   // 40유닛 뒤에서 완전 소실

    CreateSnowTex();
}

/* -------- 창 크기 변경 -------- */
void ChangeSize(int w, int h)
{
    if (h == 0) h = 1;
    float asp = (float)w / (float)h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, asp, 1.0f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("GLU Snowman");

    SetupRC();

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutMainLoop();
    return 0;
}
