
#define _USE_MATH_DEFINES
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define NUM_SPHERES 50
#define TEXSIZE   256
#define CUBESIZE  128                  // cube-map 해상도
GLuint gCubeTexID = 0;                 // 큐브맵 텍스처 (GL_TEXTURE_CUBE_MAP)

GLfloat yRot = 0.0f;

/* ---------------- Frame(카메라/오브젝트) ---------------- */
struct Frame {
    float pos[3], fwd[3], up[3];
    Frame() {
        pos[0] = pos[1] = 0; pos[2] = 0;
        fwd[0] = 0; fwd[1] = 0; fwd[2] = -1;
        up[0] = 0; up[1] = 1; up[2] = 0;
    }
    void ApplyCamera() {
        float x[3], y[3], z[3];
        z[0] = -fwd[0]; z[1] = -fwd[1]; z[2] = -fwd[2];
        x[0] = up[1] * z[2] - up[2] * z[1];
        x[1] = up[2] * z[0] - up[0] * z[2];
        x[2] = up[0] * z[1] - up[1] * z[0];
        y[0] = z[1] * x[2] - z[2] * x[1];
        y[1] = z[2] * x[0] - z[0] * x[2];
        y[2] = z[0] * x[1] - z[1] * x[0];
        float m[16] = { x[0],y[0],z[0],0,
                     x[1],y[1],z[1],0,
                     x[2],y[2],z[2],0,
                     0,0,0,1 };
        glMultMatrixf(m);
        glTranslatef(-pos[0], -pos[1], -pos[2]);
    }
    void Move(float d) { pos[0] += fwd[0] * d; pos[1] += fwd[1] * d; pos[2] += fwd[2] * d; }
    void TurnY(float a) {
        float c = cos(a), s = sin(a);
        float nf[3] = { fwd[0] * c + fwd[2] * s, fwd[1], -fwd[0] * s + fwd[2] * c };
        fwd[0] = nf[0]; fwd[1] = nf[1]; fwd[2] = nf[2];
    }
    void Set(float x, float y, float z) { pos[0] = x; pos[1] = y; pos[2] = z; }
    void Apply() { glTranslatef(pos[0], pos[1], pos[2]); }
} spheres[NUM_SPHERES], cam;

/* ---------- 큐브 맵 = 하늘색 그라디언트(절차적) ---------- */
static void MakeCubeFace(GLenum face)
{
    GLubyte img[CUBESIZE][CUBESIZE][3];
    for (int y = 0; y < CUBESIZE; ++y) {
        for (int x = 0; x < CUBESIZE; ++x) {
            float t = (float)y / (CUBESIZE - 1);       // 0(top)…1(bottom)
            GLubyte r = (GLubyte)(100 + 55 * t);       // 하늘->지평선 그라디언트
            GLubyte g = (GLubyte)(150 + 80 * t);
            GLubyte b = (GLubyte)(200 + 55 * t);
            img[y][x][0] = r; img[y][x][1] = g; img[y][x][2] = b;
        }
    }
    glTexImage2D(face, 0, GL_RGB, CUBESIZE, CUBESIZE, 0,
        GL_RGB, GL_UNSIGNED_BYTE, img);
}
static void CreateCubeMap()
{
    glGenTextures(1, &gCubeTexID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gCubeTexID);

    MakeCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    MakeCubeFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    MakeCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    MakeCubeFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    MakeCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    MakeCubeFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

/* ---- 토러스(텍스처 좌표 X, 대신 노말만) ---- */
void PlainTorus(float inR, float outR, int sides, int rings)
{
    const float TWOPI = (float)(M_PI * 2.0);
    float ringStep = TWOPI / rings, sideStep = TWOPI / sides;
    for (int r = 0; r < rings; ++r) {
        float u0 = r * ringStep, u1 = (r + 1) * ringStep;
        glBegin(GL_QUAD_STRIP);
        for (int s = 0; s <= sides; ++s) {
            float v = s * sideStep, cs = cosf(v), sn = sinf(v);
            float r0 = outR + inR * cs;
            float x0 = r0 * cosf(u0), y0 = r0 * sinf(u0), z0 = inR * sn;
            float nx0 = cs * cosf(u0), ny0 = cs * sinf(u0), nz0 = sn;
            glNormal3f(nx0, ny0, nz0); glVertex3f(x0, y0, z0);

            float x1 = (outR + inR * cs) * cosf(u1);
            float y1 = (outR + inR * cs) * sinf(u1);
            glNormal3f(nx0, ny0, nz0); glVertex3f(x1, y1, z0);
        }
        glEnd();
    }
}

/* ---- 그라운드, 스피어, 토러스 ---- */
void DrawGround()
{
    const int   EXT = 20;      // 한쪽 끝-좌표
    const int   STEP = 1;       // 격자 간격
    const float Y = -0.4f;   // 바닥 높이

    glBegin(GL_QUADS);
    for (int i = -EXT; i < EXT; i += STEP)
        for (int j = -EXT; j < EXT; j += STEP)
        {
            // 체커보드 색상
            if (((i + EXT) + (j + EXT)) & 1)
                glColor4f(0.6f, 0.6f, 0.6f, 0.6f);  // 밝은 칸
            else
                glColor4f(0.3f, 0.3f, 0.3f, 0.6f);  // 어두운 칸

            glVertex3f(i, Y, j);
            glVertex3f(i + STEP, Y, j);
            glVertex3f(i + STEP, Y, j + STEP);
            glVertex3f(i, Y, j + STEP);
        }
    glEnd();
}

void DrawWorld()
{
    /* 구들 */
    for (int i = 0; i < NUM_SPHERES; i++) {
        glPushMatrix(); spheres[i].Apply(); glutSolidSphere(.2, 13, 26); glPopMatrix();
    }
    /* 큐브-맵 토러스 + 위성 구 */
    glPushMatrix();
    glTranslatef(0, .15, -2.5);

    glPushMatrix(); glRotatef(-yRot * 2, 0, 1, 0); glTranslatef(1, 0, 0);
    glutSolidSphere(.1, 13, 26);
    glPopMatrix();

    /* ――― 반사 토러스 ――― */
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gCubeTexID);

    /* 자동 텍스쳐 좌표: 반사 벡터 = 2(N·E)N - E  */
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);

    glRotatef(yRot, 0, 1, 0);
    PlainTorus(.15, .35, 32, 32);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glPopMatrix();
}

/* ---- Render ---- */
void RenderScene() {
    yRot += 0.5f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix(); cam.ApplyCamera();

    GLfloat lightP[] = { 0,5,5,1 }, lightM[] = { 0,-5.8f,5,1 };
    /* 1) 거울 반사 월드 */
    glLightfv(GL_LIGHT0, GL_POSITION, lightM);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    for (int i = 0; i < NUM_SPHERES; ++i) {
        float ry = -0.8f - spheres[i].pos[1];   // 바닥 y = -0.4 → 대칭 = -0.8 − y
        glPushMatrix();
        glTranslatef(spheres[i].pos[0], ry + 0.0001f, spheres[i].pos[2]);
        glutSolidSphere(0.2f, 13, 26);
        glPopMatrix();
    }

    float base = .15f, refl = -.8f - base;
    glPushMatrix(); glTranslatef(0, refl, -2.5);
    glPushMatrix(); glRotatef(-yRot * 2, 0, 1, 0); glTranslatef(1, 0, 0);
    glutSolidSphere(.1, 13, 26); glPopMatrix();
    glRotatef(yRot, 0, 1, 0);
    glEnable(GL_TEXTURE_CUBE_MAP); glBindTexture(GL_TEXTURE_CUBE_MAP, gCubeTexID);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glEnable(GL_TEXTURE_GEN_S); glEnable(GL_TEXTURE_GEN_T); glEnable(GL_TEXTURE_GEN_R);
    PlainTorus(.15, .35, 32, 32);
    glDisable(GL_TEXTURE_GEN_S | GL_TEXTURE_GEN_T | GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glPopMatrix();
    glDepthMask(GL_TRUE); 
    glDisable(GL_BLEND);

    /* 2) 바닥 */
    glDisable(GL_LIGHTING); glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); DrawGround();
    glDisable(GL_BLEND); glEnable(GL_LIGHTING);

    /* 3) 정방향 월드 */
    glLightfv(GL_LIGHT0, GL_POSITION, lightP);
    DrawWorld();

    glPopMatrix();
    glutSwapBuffers();
}

/* ---- Timer & Keys ---- */
void Timer(int) { glutPostRedisplay(); glutTimerFunc(33, Timer, 1); }
void Keys(int k, int, int) {
    if (k == GLUT_KEY_UP) cam.Move(.1f);
    if (k == GLUT_KEY_DOWN)cam.Move(-.1f);
    if (k == GLUT_KEY_LEFT)cam.TurnY(.1f);
    if (k == GLUT_KEY_RIGHT)cam.TurnY(-.1f);
}

/* ---- GL 초기화 ---- */
void SetupRC() {
    GLfloat fog[] = { .1,.1,.1,1 }; glClearColor(fog[0], fog[1], fog[2], fog[3]);
    glEnable(GL_FOG); glFogfv(GL_FOG_COLOR, fog); glFogf(GL_FOG_START, 2); glFogf(GL_FOG_END, 40);
    glEnable(GL_DEPTH_TEST); glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    GLfloat Lpos[] = { 0,5,5,1 }, Lamb[] = { .1,.1,.1,1 }, Ldif[] = { .7,.7,.7,1 }, Lspec[] = { 1,1,1,1 };
    glLightfv(GL_LIGHT0, GL_POSITION, Lpos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, Lamb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Ldif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Lspec);

    GLfloat mAmb[] = { .5,.5,.5,1 }, mDif[] = { .6,.6,.6,1 }, mSpec[] = { .9,.9,.9,1 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mSpec);
    GLfloat shin[] = { 64 }; glMaterialfv(GL_FRONT, GL_SHININESS, shin);

    glEnable(GL_COLOR_SUM);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

    CreateCubeMap();                   // ★ 큐브맵 생성

    /* 구들 위치 난수 */
    for (int i = 0; i < NUM_SPHERES; i++) {
        float x = ((rand() % 400) - 200) * .1f;
        float z = ((rand() % 400) - 200) * .1f;
        spheres[i].Set(x, 0, z);
    }
}

/* ---- Viewport ---- */
void Reshape(int w, int h) {
    if (!h)h = 1; float asp = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(45, asp, 1, 425);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}

/* ---- main ---- */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Cube-Map Reflected Torus Demo");

    SetupRC();
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(Reshape);
    glutSpecialFunc(Keys);
    glutTimerFunc(33, Timer, 1);
    glutMainLoop();
    return 0;
}
