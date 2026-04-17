/********************************************************************/
/*  Mirror Reflection + Wood-Textured Torus + Separate Specular     */
/*  필요 헤더 : windows.h, gl.h, glu.h, glut.h, math.h, stdlib.h    */
/*  빌드 예시(MinGW)                                                */
/*     g++ wood_torus.cpp -o wood_torus.exe                         */
/*         -lopengl32 -lglu32 -lfreeglut                           */
/********************************************************************/
#define _USE_MATH_DEFINES
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define NUM_SPHERES 50
#define TEXSIZE     256          // ★★★ (wood texture size)
GLuint  gWoodTexID = 0;          // ★★★

GLfloat yRot = 0.0f;

/* ---------------- Frame 구조체 (카메라/배열 오브젝트) ---------------- */
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
        float x[3], y[3], z[3];
        z[0] = -forward[0]; z[1] = -forward[1]; z[2] = -forward[2];
        /* x = up × z */
        x[0] = up[1] * z[2] - up[2] * z[1];
        x[1] = up[2] * z[0] - up[0] * z[2];
        x[2] = up[0] * z[1] - up[1] * z[0];
        /* y = z × x */
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
    void MoveForward(float d) {
        pos[0] += forward[0] * d;
        pos[1] += forward[1] * d;
        pos[2] += forward[2] * d;
    }
    void RotateLocalY(float a) {
        float c = cos(a), s = sin(a);
        float nf[3] = {
            forward[0] * c + forward[2] * s,
            forward[1],
            -forward[0] * s + forward[2] * c
        };
        forward[0] = nf[0]; forward[1] = nf[1]; forward[2] = nf[2];
    }
    void SetOrigin(float x, float y, float z) { pos[0] = x; pos[1] = y; pos[2] = z; }
    void ApplyActorTransform() { glTranslatef(pos[0], pos[1], pos[2]); }
};

Frame spheres[NUM_SPHERES];
Frame frameCamera;

/* ---------------- 나무결 텍스처 생성 ----------------- */
// 단순 원형-링+노이즈 함수로 만든 procedural wood
static void MakeWoodTexture(GLubyte img[TEXSIZE][TEXSIZE][3])
{
    const float rings = 12.0f;      // 링 조밀도
    const float perturb_scale = 0.2f;       // 불규칙도
    for (int y = 0; y < TEXSIZE; ++y) {
        for (int x = 0; x < TEXSIZE; ++x) {
            // 좌표를 [-1,1] 범위로 정규화
            float fx = (2.0f * x) / TEXSIZE - 1.0f;
            float fy = (2.0f * y) / TEXSIZE - 1.0f;
            // 반지름 & 링 패턴
            float dist = sqrtf(fx * fx + fy * fy);
            float ring = dist * rings +
                perturb_scale * (rand() % 1000) / 1000.0f;
            float shade = 0.5f + 0.5f * sinf(ring * 2.0f * M_PI);
            // 베이스 갈색(90, 58, 34) ~ 밝은 갈색(190, 140, 90)
            GLubyte r = (GLubyte)(90 + shade * 100);
            GLubyte g = (GLubyte)(58 + shade * 82);
            GLubyte b = (GLubyte)(34 + shade * 56);
            img[y][x][0] = r; img[y][x][1] = g; img[y][x][2] = b;
        }
    }
}

/* ------------- 텍스처 좌표가 있는 토러스 그리기 --------------- */
/* ※ glutSolidTorus 는 텍스처 좌표를 생성하지 않으므로 교체      */
void TexturedTorus(float innerR, float outerR, int sides, int rings)
{
    const float twopi = (float)(M_PI * 2.0);
    float ringFactor = twopi / rings;
    float sideFactor = twopi / sides;

    for (int ring = 0; ring < rings; ++ring) {
        float u0 = ring * ringFactor;
        float u1 = (ring + 1) * ringFactor;
        glBegin(GL_QUAD_STRIP);
        for (int side = 0; side <= sides; ++side) {
            float v = side * sideFactor;
            float cs = cosf(v), sn = sinf(v);

            /* 두 링에 대해 두 정점 */
            float r0 = outerR + innerR * cs;
            float r1 = outerR + innerR * cs;

            /* 정점 0 */
            float x0 = r0 * cosf(u0);
            float y0 = r0 * sinf(u0);
            float z0 = innerR * sn;
            float nx0 = cs * cosf(u0);
            float ny0 = cs * sinf(u0);
            float nz0 = sn;
            glNormal3f(nx0, ny0, nz0);
            glTexCoord2f((float)side / sides, (float)ring / rings);  // ★★★
            glVertex3f(x0, y0, z0);

            /* 정점 1 */
            float x1 = r1 * cosf(u1);
            float y1 = r1 * sinf(u1);
            float z1 = z0;
            float nx1 = cs * cosf(u1);
            float ny1 = cs * sinf(u1);
            float nz1 = nz0;
            glNormal3f(nx1, ny1, nz1);
            glTexCoord2f((float)side / sides, (float)(ring + 1) / rings); // ★★★
            glVertex3f(x1, y1, z1);
        }
        glEnd();
    }
}

/* ---------------- 체커 보드 바닥 ---------------- */
void DrawGround(void)
{
    const GLfloat fExtent = 20.0f;
    const GLfloat fStep = 1.0f;
    const GLfloat y = -0.4f;

    glBegin(GL_QUADS);
    for (int i = -fExtent; i < fExtent; i += fStep) {
        for (int j = -fExtent; j < fExtent; j += fStep) {
            if (((int)(i + fExtent) + (int)(j + fExtent)) & 1)
                glColor4f(0.6f, 0.6f, 0.6f, 0.6f);
            else glColor4f(0.3f, 0.3f, 0.3f, 0.6f);
            glVertex3f(i, y, j);
            glVertex3f(i + fStep, y, j);
            glVertex3f(i + fStep, y, j + fStep);
            glVertex3f(i, y, j + fStep);
        }
    }
    glEnd();
}

/* --------------- 나머지 월드(구/토러스 등) ---------------- */
void DrawWorld()
{
    for (int i = 0; i < NUM_SPHERES; i++) {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        glutSolidSphere(0.2f, 13, 26);
        glPopMatrix();
    }

    /* 나무 토러스 + 회전 구 */
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, -2.5f);

    /* 회전 작은 구 */
    glPushMatrix();
    glRotatef(-yRot * 2.0f, 0, 1, 0);
    glTranslatef(1.0f, 0.0f, 0.0f);
    glutSolidSphere(0.1f, 13, 26);
    glPopMatrix();

    /* ★★★ 텍스처링 켜기 */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gWoodTexID);

    glRotatef(yRot, 0, 1, 0);
    TexturedTorus(0.15f, 0.35f, 32, 32);  // 텍스처 좌표 포함 토러스

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/* ----------------------------- RenderScene ---------------------------- */
void RenderScene()
{
    yRot += 0.5f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    frameCamera.ApplyCameraTransform();

    GLfloat lightPos[] = { 0.0f,5.0f,5.0f,1.0f };
    GLfloat lightPosMirror[] = { 0.0f,-5.8f,5.0f,1.0f };

    /* [1] 거울에 비친 월드 ------------------------------------ */
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosMirror);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);

    /* (a) 거울에 비친 구들 */
    for (int i = 0; i < NUM_SPHERES; i++) {
        float ry = -0.4f * 2 - spheres[i].pos[1]; // y 대칭
        glPushMatrix();
        glTranslatef(spheres[i].pos[0], ry + 0.0001f, spheres[i].pos[2]);
        glutSolidSphere(0.2f, 13, 26);
        glPopMatrix();
    }
    /* (b) 거울 속 토러스 */
    float baseY = 0.15f, reflY = 2 * -0.4f - baseY;
    glPushMatrix();
    glTranslatef(0.0f, reflY + 0.0001f, -2.5f);
    glPushMatrix();
    glRotatef(-yRot * 2.0f, 0, 1, 0);
    glTranslatef(1.0f, 0, 0);
    glutSolidSphere(0.1f, 13, 26);
    glPopMatrix();
    glRotatef(yRot, 0, 1, 0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gWoodTexID);
    TexturedTorus(0.15f, 0.35f, 32, 32);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    /* [2] 반투명 바닥 ---------------------------------------- */
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    DrawGround();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    /* [3] 정방향 월드 ---------------------------------------- */
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    DrawWorld();

    glPopMatrix();
    glutSwapBuffers();
}

/* --------------------------- Timer & Input --------------------------- */
void TimerFunction(int) { glutPostRedisplay(); glutTimerFunc(33, TimerFunction, 1); }
void SpecialKeys(int key, int, int) {
    if (key == GLUT_KEY_UP)   frameCamera.MoveForward(0.1f);
    if (key == GLUT_KEY_DOWN) frameCamera.MoveForward(-0.1f);
    if (key == GLUT_KEY_LEFT) frameCamera.RotateLocalY(0.1f);
    if (key == GLUT_KEY_RIGHT)frameCamera.RotateLocalY(-0.1f);
    glutPostRedisplay();
}

/* --------------------------- SetupRC --------------------------------- */
void SetupRC()
{
    /* ── 안개(배경) 색 ── */
    GLfloat fogColor[] = { 0.1f,0.1f,0.1f,1.0f };
    glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, 1.0f);
    glFogf(GL_FOG_END, 30.0f);
    glFogi(GL_FOG_MODE, GL_LINEAR);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    /* ── 라이팅 ── */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = { 0.0f,5.0f,5.0f,1.0f };
    GLfloat lightAmb[] = { 0.1f,0.1f,0.1f,1.0f };
    GLfloat lightDif[] = { 0.7f,0.7f,0.7f,1.0f };
    GLfloat lightSpec[] = { 1.0f,1.0f,1.0f,1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

    /* ── 머티리얼 ── */
    GLfloat matAmb[] = { 0.2f,0.2f,0.2f,1.0f };
    GLfloat matDif[] = { 0.6f,0.6f,0.6f,1.0f };
    GLfloat matSpec[] = { 0.8f,0.8f,0.8f,1.0f };
    GLfloat matSh[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matSh);

    /* ★★★ 1) ‘Separate Specular Color’ 활성화 */
    glEnable(GL_COLOR_SUM); //   primary + secondary 컬러합 사용
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

    /* ★★★ 2) 나무결 텍스처 생성·업로드 */
    GLubyte img[TEXSIZE][TEXSIZE][3];
    MakeWoodTexture(img);

    glGenTextures(1, &gWoodTexID);
    glBindTexture(GL_TEXTURE_2D, gWoodTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXSIZE, TEXSIZE, 0,
        GL_RGB, GL_UNSIGNED_BYTE, img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* ── 랜덤 위치 구들 ── */
    for (int i = 0; i < NUM_SPHERES; i++) {
        float x = ((rand() % 400) - 200) * 0.1f;
        float z = ((rand() % 400) - 200) * 0.1f;
        spheres[i].SetOrigin(x, 0.0f, z);
    }
}

/* --------------------------- Viewport ------------------------------- */
void ChangeSize(int w, int h)
{
    if (!h)h = 1;
    GLfloat asp = (GLfloat)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, asp, 1.0f, 425.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* --------------------------- main ----------------------------------- */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Mirror Reflection – Wood Textured Torus");

    SetupRC();

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutTimerFunc(33, TimerFunction, 1);

    glutMainLoop();
    return 0;
}
