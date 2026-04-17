# Part 1 Classic OpenGL

# 3D 그래픽과 OpenGL

즉시 모드와 보류 모드

### 보류 모드(retained mode)

물체, 장면 정보를 API에 제공하면, 내부적인 처리를 거쳐 화면에 출력

모든 물체를 씬그래프에 넣어두고, 추가적인 정보만 제공해 화면을 그려냄.

### 즉시모드(immediate mode)

API가 이 방식으로 이루어져있으며, 그래픽 프로세서에 직접적인 명령을 전달해 그 상태를 변경하고, 이어지는 모든 명령에 영향을 준다.

이미 실행된 명령에 대해서는 그 후 내린 명령이 아무런 영향을 주지 못한다.

### 뷰포트(ViewPort)

클리핑 영역과 창의 픽셀수가 정확히 맞아떨어지지않기때문에

논리적 직교 좌표를 물리적인 화면 픽셀 좌표로 전환하는 과정에 쓰는 것이 뷰포트

### 투영(Projection)

3D 좌표계에서 만들어진 지오메트리를 편평한 2D 공간으로 옮기는것

## OpenGL

그래픽 하드웨어 제어를 위한 소프트웨어 인터페이스

모태는 실리콘그래픽스의 IRIS GL

하드웨어의 3D 가속 기능을 사용한 Implementation , 그렇지 않은 implementation으로 나눠짐

## DirectX

보다 openGL이 훨씬 편리하다

opengl32.dll에 들어있음.(System Directory) (3D 렌더링 등 지원)

glu32.dll(GLU 오픈gl 유틸리티 라이브러리)(행렬계산, 여러 기능 지원)

gl.h (모든 openGL 함수, 타입, 매크로)

| GLbyte | 8비트 정수 |
| --- | --- |
| GLshort | 16비트 정수 |
| GLint, GLsizei | 32비트 정수 |
| GLfloat, GLclampf | 32비트 실수 |
| GLdouble, GLclampd | 64비트 실수 |
| GLubyte, GLboolean | 8비트 부호없는 정수 |
| GLushort | 16비트 부호없는 정수 |
| GLunit, GLenum, GLbitfield | 32비트 부호없는 정수 |

### openGL 함수 이름짓기 규칙

<라이브러리 접두어><루트명령어>선택적인 인자수><선택적인 인자타입>

gl - Color - 3f - (….)

glColor3f()

3개의 float 인자를 가지는 Color

### float와 double

많은 c/c++ 컴파일러들이 float를 double로 인식

float 타입임을 명확히 지정해주자

어차피 openGL은 내부적으로 float를 쓰므로..

두배 메모리 사용을 막기 위해서 명시해주자

## 플랫폼 독립성

openGL은 창, 화면관리를 위한 함수나 명령어가 한개도없음

키보드 입력, 마우스 제어 함수도 전혀없다.

→ openGL 디자이너의 최종 목표가 플랫폼 독립성이기때문

### GLUT(OpenGL Utility Toolkit)

GLU는 OpenGl Utility Library이니 헷갈리지 말 것

팜업메뉴, 창 관리 등… 윈도우에 맞게 쓸수있는 툴!!



--------------------------------------
# 실습
![image](https://github.com/user-attachments/assets/8db4f374-c187-4b29-9b10-f51b30476193)


# 2-2
resize Squere (set aspect Ratio) 
![image](https://github.com/user-attachments/assets/fe0657ea-5b52-4f77-9820-e245052124ee)

![image](https://github.com/user-attachments/assets/cc2a7431-b2b3-4b72-bce5-d51130522aad)


## 만약 Resize와 동시에 좌표계를 변경하지않는다면..
![image](https://github.com/user-attachments/assets/e1d2a228-50ea-493a-ad27-3a3f55ac04f6)




## Move BOX!


	glutTimerFunc(33, TimerFunction, 1);  <- 시간 호출로 움직이게 구현


### 싱글 버퍼
https://github.com/user-attachments/assets/58eecfa6-113a-4cf9-b576-ea38bb5fb130

### DOUBLE BUFFER
https://github.com/user-attachments/assets/9807b107-5b55-4aae-bffc-d8f2ec8cc345

## 더블 버퍼링?
스크린 버퍼에 렌더링이 이루어지는동안 드로잉 코드를 실행하는 것

1) 렌더링 시간이 걸릴때 : 이미지가 그려지는 모습을 사용자에게 보여주지않고 한번에 보여주기위해서
2) 애니메이션 : 프레임을 그려두었다가 프레임을 바꾸어가며 애니메이션 효과 내기


glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
->>>
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

이때, 버퍼가 교체될때 드로잉 결과가 출력되므로 간접적으로 glFlush가 이뤄지고있음!!


# OpenGL State Machine

3D 그래픽에서 최종 결과물에 영향을 미치는 다양한 설정들을 OpenGL에서는 **state(상태)**라고 부릅니다. 이 state는 파이프라인 전반에 걸쳐 존재하며, 개발자는 이를 제어하고 저장/복구할 수 있습니다.

---

## 상태 제어 함수

```c
void glEnable(GLenum cap);        // 상태 활성화
void glDisable(GLenum cap);       // 상태 비활성화
GLboolean glIsEnabled(GLenum cap); // 상태 확인
```

예시 사용값:
- `GL_DEPTH_TEST`
- `GL_LIGHTING`
- `GL_TEXTURE_2D` 등

---

## 상태값 조회 함수

OpenGL 상태값은 다양한 타입으로 조회할 수 있습니다.

```c
void glGetBooleanv(GLenum pname, GLboolean *params);
void glGetDoublev(GLenum pname, GLdouble *params);
void glGetFloatv(GLenum pname, GLfloat *params);
void glGetIntegerv(GLenum pname, GLint *params);
```

---

## OpenGL STATE MACHINE 상태 스택 저장/복구

OpenGL은 특정 상태들을 스택 구조로 저장하고 복원할 수 있습니다.

```c
void glPushAttrib(GLbitfield mask);  // 상태 저장
void glPopAttrib(void);              // 상태 복원
```

- `GLbitfield`는 여러 상태를 OR 연산으로 합칠 수 있는 비트 플래그입니다.
- 예시:

```c
glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT);
```

---

## OpenGL Error Flags

OpenGL은 함수 호출 오류 시 예외를 발생시키지 않고, 내부 오류 플래그에 기록합니다.

```c
GLenum glGetError(void);
```

### 반환 가능한 오류 코드:

| 코드 | 설명 |
|------|------|
| `GL_NO_ERROR` | 오류 없음 |
| `GL_INVALID_ENUM` | 잘못된 열거형 인자 |
| `GL_INVALID_VALUE` | 잘못된 수치 인자 |
| `GL_INVALID_OPERATION` | 현재 상태에 맞지 않는 명령어 |
| `GL_STACK_OVERFLOW` | 상태 스택 오버플로우 |
| `GL_STACK_UNDERFLOW` | 상태 스택 언더플로우 |
| `GL_OUT_OF_MEMORY` | 메모리 부족 |
| `GL_TABLE_TOO_LARGE` | 테이블이 너무 큼 |

### 일반적인 오류 검사 패턴:

```c
while (glGetError() != GL_NO_ERROR) {
    // 오류 처리
}
```

---

## gluErrorString - 오류 메시지 반환

OpenGL의 보조 라이브러리 GLU는 오류 코드를 문자열로 바꿔주는 기능을 제공합니다.

```c
const GLubyte* gluErrorString(GLenum errorCode);
```

예시:
- `GL_INVALID_ENUM` → `"invalid enumerant"`
- 대부분의 OpenGL 오류는 해당 줄을 **무시하고 넘어감**.
- 단, 포인터 인자를 사용하는 함수에서 **잘못된 메모리 접근** 시에는 **프로그램이 즉시 종료**


# OpenGL 함수 레퍼런스

OpenGL에서 자주 사용되는 주요 함수들을 범주별로 정리한 문서입니다. (Fixed-function pipeline 기준)

---

## 상태 제어 관련 (State Management)

| 함수 | 설명 |
|------|------|
| `glEnable(GLenum cap)` | 특정 기능 활성화 |
| `glDisable(GLenum cap)` | 특정 기능 비활성화 |
| `glIsEnabled(GLenum cap)` | 기능이 켜져 있는지 확인 |
| `glGetBooleanv`, `glGetFloatv`, `glGetIntegerv`, `glGetDoublev` | 상태값 조회 |
| `glPushAttrib(GLbitfield mask)` | 상태 스택에 저장 |
| `glPopAttrib()` | 스택에서 상태 복원 |

---

## 렌더링 관련 (Rendering)

| 함수 | 설명 |
|------|------|
| `glBegin(GLenum mode)` | 도형 그리기 시작 (`GL_TRIANGLES`, `GL_QUADS` 등) |
| `glEnd()` | 도형 그리기 종료 |
| `glVertex*()` | 꼭짓점 지정 |
| `glColor*()` | 색상 지정 |
| `glNormal*()` | 노멀 벡터 지정 |
| `glTexCoord*()` | 텍스처 좌표 지정 |
| `glRectf()` | 사각형 그리기 (편의 함수) |
| `glFlush()` | 명령 실행 보장 (싱글 버퍼용) |
| `glFinish()` | 모든 명령 실행 완료까지 대기 |

---

## 변환 및 행렬 관련 (Transformations)

| 함수 | 설명 |
|------|------|
| `glMatrixMode(GLenum mode)` | 현재 사용할 행렬 설정 (`GL_MODELVIEW`, `GL_PROJECTION`) |
| `glLoadIdentity()` | 단위 행렬로 초기화 |
| `glPushMatrix()` / `glPopMatrix()` | 현재 행렬 스택에 저장/복원 |
| `glTranslatef(x, y, z)` | 평행 이동 |
| `glRotatef(angle, x, y, z)` | 회전 |
| `glScalef(x, y, z)` | 크기 조절 |
| `glLoadMatrixf()` | 사용자 행렬 불러오기 |
| `glMultMatrixf()` | 현재 행렬에 곱셈 |

---

## 조명 및 재질 관련 (Lighting & Materials)

| 함수 | 설명 |
|------|------|
| `glEnable(GL_LIGHTING)` | 조명 활성화 |
| `glEnable(GL_LIGHT0)` | 개별 광원 활성화 |
| `glLightfv()` | 광원 속성 설정 |
| `glMaterialfv()` | 재질 속성 설정 |
| `glShadeModel(GL_FLAT / GL_SMOOTH)` | 음영 처리 방식 설정 |

---

## 텍스처 관련 (Texture Mapping)

| 함수 | 설명 |
|------|------|
| `glEnable(GL_TEXTURE_2D)` | 2D 텍스처 활성화 |
| `glTexImage2D()` | 텍스처 이미지 업로드 |
| `glBindTexture()` | 텍스처 바인딩 |
| `glTexParameteri()` | 텍스처 필터링/랩핑 설정 |
| `glTexEnvf()` | 텍스처 환경 설정 |
| `glGenTextures() / glDeleteTextures()` | 텍스처 생성 및 삭제 |

---

## 뷰포트 및 투영 설정 (Viewport & Projection)

| 함수 | 설명 |
|------|------|
| `glViewport(x, y, width, height)` | 뷰포트 설정 |
| `glOrtho()` | 직교 투영 |
| `gluPerspective()` | 원근 투영 |
| `gluLookAt()` | 카메라 위치 설정 |

---

## 오류 처리 관련

| 함수 | 설명 |
|------|------|
| `glGetError()` | 에러 상태 플래그 반환 |
| `gluErrorString()` | 에러 코드 → 문자열로 변환 |

---

## 기타

| 함수 | 설명 |
|------|------|
| `glClearColor(r, g, b, a)` | 배경색 설정 |
| `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)` | 버퍼 초기화 |
| `glDepthFunc()` | 깊이 테스트 설정 |
| `glCullFace()` | 컬링 설정 (앞면/뒷면) |
| `glPolygonMode()` | 폴리곤 그리기 방식 설정 |

---

## 참고 링크

- OpenGL 2.1 Quick Reference Card: https://www.khronos.org/files/opengl-2.1-quick-reference-card.pdf  
- OpenGL Red Book: https://www.glprogramming.com/red/



  
