// Toon_Headlight.cpp
// 헤드라이트(카메라 고정) 툰 셰이딩 데모
// g++ Toon_Headlight.cpp -o Toon.exe -lopengl32 -lglu32 -lfreeglut

#define _USE_MATH_DEFINES
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

// ───────────── Vector / Matrix helper ─────────────
static void vecNormalize(float v[3])
{
    float len = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (len > 1e-8f) { v[0] /= len; v[1] /= len; v[2] /= len; }
}
static float vecDot(const float a[3], const float b[3])
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
// Rᵀ·v  (현재 MODELVIEW 상단 3×3의 전치만 적용 = 역회전)
static void inverseRotateVector(const float mv[16],
    const float v[3], float out[3])
{
    out[0] = v[0] * mv[0] + v[1] * mv[4] + v[2] * mv[8];
    out[1] = v[0] * mv[1] + v[1] * mv[5] + v[2] * mv[9];
    out[2] = v[0] * mv[2] + v[1] * mv[6] + v[2] * mv[10];
}

// ───────────── Global state ─────────────
static const float gHeadLightEye[3] = { 0.0f, 0.0f, 1.0f }; // 카메라 고정 광원
static float       gYRot = 0.0f;                            // 토러스 회전각

// ───────────── Torus with toon shading ─────────────
static void drawTorusToon(float majorR, float minorR,
    int numMajor, int numMinor)
{
    // 1) MODELVIEW(=카메라·모델) 얻기
    float mv[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv);

    // 2) 헤드라이트 벡터를 ‘객체 공간’으로 역회전
    float lightObj[3];
    inverseRotateVector(mv, gHeadLightEye, lightObj);
    vecNormalize(lightObj);                 // (방향 벡터이므로 translation 無)

    // 3) 토러스 메시
    const double majorStep = 2.0 * M_PI / numMajor;
    const double minorStep = 2.0 * M_PI / numMinor;
    for (int i = 0; i < numMajor; ++i)
    {
        double a0 = i * majorStep, a1 = a0 + majorStep;
        float x0 = (float)cos(a0), y0 = (float)sin(a0);
        float x1 = (float)cos(a1), y1 = (float)sin(a1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= numMinor; ++j)
        {
            double b = j * minorStep;
            float  c = (float)cos(b);
            float  r = minorR * c + majorR;
            float  z = minorR * (float)sin(b);

            float n[3];

            n[0] = x0 * c; n[1] = y0 * c; n[2] = z / minorR;
            vecNormalize(n);
            glTexCoord1f(vecDot(lightObj, n));  // 내적→1-D tex coord
            glVertex3f(x0 * r, y0 * r, z);

            n[0] = x1 * c; n[1] = y1 * c; n[2] = z / minorR;
            vecNormalize(n);
            glTexCoord1f(vecDot(lightObj, n));
            glVertex3f(x1 * r, y1 * r, z);
        }
        glEnd();
    }
}

// ───────────── Callbacks ─────────────
static void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(0, 0, -2.5f);            // 모델 위치
    glRotatef(gYRot, 0, 1, 0);             // 모델 회전
    drawTorusToon(0.35f, 0.15f, 50, 25);
    glPopMatrix();

    glutSwapBuffers();
    gYRot += 0.01f;
}
static void reshape(int w, int h)
{
    if (!h)h = 1;
    float asp = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(35, asp, 1, 100);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}
static void setupRC()
{
    // 초록 4-단계 1-D 텍스처
    GLubyte toon[4][3] = { {0,32,0},{0,64,0},{0,128,0},{0,192,0} };
    glClearColor(0, 0, 0.5f, 1);
    glEnable(GL_DEPTH_TEST); glEnable(GL_CULL_FACE);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4, 0,
        GL_RGB, GL_UNSIGNED_BYTE, toon);
    glEnable(GL_TEXTURE_1D);
}

// ───────────── main ─────────────
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Toon Shading – Head-light");

    setupRC();
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
