# Part 1 Classic OpenGL
## geomtric transformation and pipeline
Modeling Transformation -> Projection Transformation(Perspective -> Orthographic) -> ViewPort Transformation

변환 파이프라인

M' = (F^(-1) M F)

  


### Vertex 이동

- void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
- glTranslatef(0.0f, 10.0f, 0.0f);

### 회전
- glRotatef(GLfloat 각도, GLfloat x, GLfloat y, GLfloat z);
- 회전은 반시계기준!!

- Scale
- glScalef(GLfloat x, GLfloat y, GLfloat z);


### 단위행렬
코드에 있는 각각의 변환은 누적된다..
그래서 각각의 실행이 영향을 미친다.

따라서 모델뷰 행렬을 단위행렬 I 로 초기화해줌으로써
현재까지의 변환 내용을 무시하고, 깨끗한 상태로 새로운 변환을 할 수 있음 

- glMatrixMode(GL_MODELVIEW);
- glLoadIdentity() // 단위 행렬로 초기화
- 변환 1
- 변환 2
- glLoadIdentity()
- 변환 3
변환 3은 아무런 영향도 받지않고 자신의 변환만 수행할수있다.

### 행렬 스택
각 물체를 지정할때마다 단위행렬로 모델뷰 행렬을 초기화하는것보다..

행렬 스택을 이용하자

모델뷰 행렬, 투영 행렬을 저장해둘 수 있는 공간으로
필요할때 꺼내 쓰는 스택이다!

- glGet(GL_MAX_MODELVIEW_STACK_DEPTH); // 모델뷰 행렬의 최대 깊이
- glGET(GL_MAX_PROJECTION_STACK_DEPTH); //투영 행렬의 최대 깊이


### 원근 투영
- void gluPerspective(GLdouble 시야각, GLdouble 종횡비, GLdouble z앞면, GLdouble z뒷면);


# Solar System lololol


https://github.com/user-attachments/assets/82c38567-7e27-46b8-b0fe-6dae6c741a0d


# 보다 발전된 형태의 행렬 제어

openGL에서는 4x4 행렬을 이차원 실수 배열이 아닌, 16개의 실수값을 가진 하나의 배열로 표현한다.
즉 수학 라이브러리와는 다르다

- GLfloat matrix[16]; // OpenGL에서 쓰기 좋은 표현
- GLfloat matrix[4][4]; // 흔히쓰지만, openGL에서는 비효율적

![image](https://github.com/user-attachments/assets/dd081ade-0a92-40fa-9dfa-15b84ec6d9e9)
열기준의 행렬정렬

16개의 값은 시각 좌표계의 세 축이 만들어내는 공간에서의 특정 위치를 표현한다.
즉 
![image](https://github.com/user-attachments/assets/72214b0b-8055-425c-b5f3-a442b74364c9)

- 처음 3개의 열은 x,y,z축에 대한 방향을 나타내는 역할을 한다.
벡터는 크기뿐만이 아닌, 방향도 내포한다.
마지막 4번째 열이 만들어내는 벡터는 변환된 좌표계의 x,y,z값을 포함한다.

재밌는건
또 다른 좌표계에서 4x4 행렬이 있을때 두 행렬을 곱해주면 새로운 좌표계로 변환된 vertex가 나온다.


# 보다 발전된 형태의 행렬 제어

openGL에서는 4x4 행렬을 이차원 실수 배열이 아닌, 16개의 실수값을 가진 하나의 배열로 표현한다.
즉 수학 라이브러리와는 다르다

- GLfloat matrix[16]; // OpenGL에서 쓰기 좋은 표현
- GLfloat matrix[4][4]; // 흔히쓰지만, openGL에서는 비효율적

![image](https://github.com/user-attachments/assets/dd081ade-0a92-40fa-9dfa-15b84ec6d9e9)
열기준의 행렬정렬

16개의 값은 시각 좌표계의 세 축이 만들어내는 공간에서의 특정 위치를 표현한다.
즉 
![image](https://github.com/user-attachments/assets/72214b0b-8055-425c-b5f3-a442b74364c9)

- 처음 3개의 열은 x,y,z축에 대한 방향을 나타내는 역할을 한다.
벡터는 크기뿐만이 아닌, 방향도 내포한다.
마지막 4번째 열이 만들어내는 벡터는 변환된 좌표계의 x,y,z값을 포함한다.

재밌는건
또 다른 좌표계에서 4x4 행렬이 있을때 두 행렬을 곱해주면 새로운 좌표계로 변환된 vertex가 나온다.

Result = A × B
이는 행렬 A에서 B로 좌표계를 변환한다는 것이다.
### 즉 OpenGL은 Column-Major-Order이다. (열 우선 정렬)


# 행렬의 로딩

- glLoadMatrixf(GLfloat m);
- glLoadMatrixd(Glfloat m);

- 대부분 float타입으로 저장하니, 두번째 줄 matrixd는 타입 변환이 따로 이뤄짐(손해)

물론 아래와같은 전치행렬을 얻어내는 함수를 쓰면
행 기준 정렬 행렬도 로딩 가능
- void glLoadTransposeMatrixf(GLfloat m);


### 4.5 torus
https://github.com/user-attachments/assets/26080997-8691-42fe-abdc-ebab4d68775a

# 카메라와 액터를 사용한 OpenGL에서의 이동

3d 장면에서 물체의 위치와 방향을 표현하기 위해서는 4x4 행렬이 필요하다.

이런 행렬 제어를 손쉽게 하는게 좋을것. 


가만히 있는 배경과 다르게 움직이는 것들은 액터(actor)라고한다.
이런 액터들은 변환이 자주 일어나기때문에 '참조 프레임' 또는 '로컬 객체 좌표계'를 가지고 있다
Reference Frame or Local Object Cordinate

### 액터 프레임
참조 프레임을 구현하는 가장 간단하고 유연한 방법은
- 공간 내 위치
- 전면을 향하는 벡터
- 위를 향하는 벡터
를 데이터 구조로 만드는 것



```c
class GLFrame
{
	protected:
	M3DVector3f vLocation;
	M3DVector3f vUp;
	M3DVector3f vForward;
public:
. . .
};

//참조 프레임에서 4x4 행렬을 받아오는 코드
// Derives a 4x4 transformation matrix from a frame of reference

void GLFrame::GetMatrix(M3DTMatrix44f mMatrix, bool bRotationOnly = false)
{
	// Calculate the right side (x) vector, drop it right into the matrix
	M3DVector3f vXAxis;
	m3dCrossProduct(vXAxis, vUp, vForward);
	// Set matrix column does not fill in the fourth value...
	m3dSetMatrixColumn44(matrix, vXAxis, 0);
	matrix[3] = 0.0f;
	// Y Column
	m3dSetMatrixColumn44(matrix, vUp, 1);
	matrix[7] = 0.0f;
	// Z Column
	m3dSetMatrixColumn44(matrix, vForward, 2);
	matrix[11] = 0.0f;
	// Translation (already done)
	if(bRotationOnly == true)
	{
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	}
	else
	m3dSetMatrixColumn44(matrix, vOrigin, 3);
	matrix[15] 
}
```

# 오일러각(Euler Angle)
yaw pitch roll

x   y     z


항공기의 위치와 방향을 나타낼때도 사용한다.


오일러 각은 불안정한 측면이 존재한다.
- 하나의 위치와 방향은 서로 다른 오일러 각으로 표현이 가능
- 특정 축을 기준으로 회전이 불가능해지는 Gimber Lock 현상 발생
- 자체적으로 정의한 축을 기준으로 회전시킬때 시야를 따라 이동하는 것만으로는 새로운 좌표 계산이 어려ㅜㅇㅁ

이에 대응하기 위해서 Quaternions을 도입하기도 한다.

# 카메라 관리
OpenGL에는 카메라 변환 기능이 없다.
단지 카메라를 공간 내의 특정 위치에서 특정한 방향을 가진 물체로 처리하면
3D 환명이 액터와 카메라에 대한 참조 프레임으로 표현이 가능하다.

```c
void gluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez,
GLdouble centerx, GLdouble centery, GLdouble centerz,
GLdouble upx, GLdouble upy, GLdouble upz);
```

### 카메라 변환은 카메라의 액터 변환을 반대로 뒤집으면 된다..!!!
![image](https://github.com/user-attachments/assets/faa25d88-c64e-4e72-8bd4-40ad0f3ba44f)


동일한 기능을 하지만 참조 프레임을 사용하는 함수도 있다.
```c
void gltApplyCameraTransform(GLTFrame *pCamera);
```


정리하자면
OpenGL에서의 "프레임(Frame)"은 단순히 하나의 위치나 방향을 넘어서, 로컬 좌표계(Local Coordinate System) 를 표현하는 구조.
즉, 어떤 객체나 카메라가 현재 바라보는 방향 + 위치 를 하나로 묶어서 표현하고, 그에 따라 변환 행렬(ModelView Matrix) 을 구성하는데 활용된다.

## 4-7 Sphere World!




https://github.com/user-attachments/assets/e0e25632-4483-4dc1-8b18-8438b18c6f88



1. 카메라 프레임 (frameCamera)
- 플레이어 시점처럼 월드를 탐색할 수 있게 해 줌
- 방향 전환 (RotateLocalY)과 이동 (MoveForward)을 프레임의 기준으로 수행
- 실제 월드가 움직이는 것이 아니라, 카메라 프레임의 위치가 변할 뿐\
  
2. 구체들의 프레임 (spheres[i])
- 각 구체마다 독립적인 로컬 좌표계를 가짐
- SetOrigin(x, 0, z)으로 특정 위치에 고정
- ApplyActorTransform()을 호출해 해당 프레임 기준의 모델 변환 적용
