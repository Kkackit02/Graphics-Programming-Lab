/********************************************************************/
/*  Simple 1-D Bézier Curve (OpenGL evaluator)                      */
/*  Build (MinGW) : g++ bezier.cpp -o bezier.exe                    */
/*                         -lopengl32 -lglu32 -lfreeglut            */
/********************************************************************/
#define _USE_MATH_DEFINES
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

/* ---------------- 제어점 ---------------- */
const int nNumPoints = 4;                          // 3차 Bézier
GLfloat ctrlPoints[nNumPoints][3] = {              // (x,y,z)
   { -8.0f, -4.0f, 0.0f },
   { -2.0f,  8.0f, 0.0f },
   {  2.0f, -8.0f, 0.0f },
   {  8.0f,  4.0f, 0.0f }
};

/* ---------------- 제어점 시각화 ---------------- */
void DrawPoints()
{
    glPointSize(6.0f);
    glColor3f(1.0f, 0.0f, 0.0f);    // 빨간 점
    glBegin(GL_POINTS);
    for (int i = 0; i < nNumPoints; ++i)
        glVertex3fv(ctrlPoints[i]);
    glEnd();
}

/* ---------------- 렌더 ---------------- */
void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    /* Bézier evaluator 설정 */
    glMap1f(GL_MAP1_VERTEX_3,          // 생성할 데이터 타입
        0.0f, 1.0f,                // u 파라미터 범위
        3,                         // 한 제어점 간 stride(float * 3)
        nNumPoints,                // 제어점 개수
        &ctrlPoints[0][0]);        // 제어점 배열 주소
    glEnable(GL_MAP1_VERTEX_3);

    /* 곡선 그리기 */
    glColor3f(0.0f, 1.0f, 0.0f);       // 초록 곡선
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 100; ++i)
        glEvalCoord1f(i / 100.0f);   // u = 0 … 1
    glEnd();

    /* 제어점 표시 */
    DrawPoints();

    glutSwapBuffers();
}

/* ---------------- 윈도 리사이즈 ---------------- */
void ChangeSize(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-10.0, 10.0, -10.0, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* ---------------- 초기화 ---------------- */
void SetupRC()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // 짙은 남청색 배경
    glShadeModel(GL_FLAT);
}

/* ---------------- main ---------------- */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("OpenGL Bézier Curve – Evaluator");

    SetupRC();
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutMainLoop();
    return 0;
}
