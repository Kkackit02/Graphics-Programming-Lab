![image](https://github.com/user-attachments/assets/1916b975-81ca-4fd3-8398-8edade04f0b1)# Part 1 Classic OpenGL
## Color and Materials
![image](https://github.com/user-attachments/assets/8c64f5aa-7374-4244-849a-165850f8ad45)

1) 색상의 블렌딩(투명, 자연스러운 색상 설정)

### 투명성?
투명을 만들기 위해...
1) 배경을 먼저 그리고
2) 앞쪽에 있는 물체와 이미 색상 버퍼 내에 있는 내용을 블렌딩
a(알파) 값을 사용해보자!


## 블렌딩
openGL에서의 렌더링은 색상 버퍼 내에 색상값을 넣는 것을 의미한다.
또한 깊이 값도 depth buffer에 넣어, 깊이 테스팅에 사용한다.

근데 glEnable(GL_BLENDING)을 키는 순간
새로 지정되는 색이 기존 색에 결합된다. (색 버퍼, 깊이 버퍼 다 의미 X)

### 색상 결합
표준적인 블렌딩 공식은
Cf = (Cs * S) + (Cd * D)
source, destination

glBlendFunc(GLenum S, GLenum D);

이때 아무거나 인자로 넣을수있는건 아니고, 
![image](https://github.com/user-attachments/assets/63a36523-0734-4428-b843-44a846e8705b)

사전 정의된 위 값들만 사용 가능


### 자주 쓰는 블렌딩 함수 조합

1) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
source 색상을 받아, RGB값을 A 값에 곱한 다음,

destination 색상의 RGB값을 (1-source의 Alpha 값)과 곱한 값에 더하는 연산 수행

-> Interpolation!!



![image](https://github.com/user-attachments/assets/79b14516-5184-45a7-b7ae-9ae9f330a77f)


## anti-aliasing
샘플링 주파수가 원본 신호의 최대 주파수의 2배보다 낮은 경우, 인접한 스펙트럼이 겹쳐서 출력이 왜곡되는 현상(멀티미디어 공학에서 배웠다!)


그래픽스에서는 픽셀을 기준으로 표현하기때문에, 픽셀 사이에서 드러나는 명확한 계단형태의 경계선을 의미한다.
이를 없애기위해

해당 픽셀의 destination 색상과 주변 픽셀의 색상을 블렌딩하는 방법을 사용한다.
즉 색 차이가 뚜렷한 부분을 흐릿하게 만드는 것.

glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
일단 블렌딩이 가능하게 설정하고,

glEnable(GL_POINT_SMOOTH); //점의 안티에일리어싱
glEnable(GL_LINE_SMOOTH); //선의 안티에일리어싱
glEnable(GL_POLYGON_SMOOTH); //폴리곤 경계의 안티에일리어싱

## Multi Sampling

-> glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);

GLUT_MULTISAMPLE!

glEnable(GL_MULTISAMPLE);
glDisable(GL_MULTISAMPLE);

멀티 샘플링을 적용하면 점, 선, 폴리곤의 smooting 기능이 모두 무시된다.
경우에 따라 multi sampling과 smooting을 잘 선택해서 구현하자

### 확실히 부드럽다!
![image](https://github.com/user-attachments/assets/9f84d8ca-c5ce-43c6-a475-a446098fa52f)


# 안개
openGL에서는 안개도 구현이 가능하다.

GLfloat flowLight[] = {0.1f, 0.1f, 0.1f, 1.0f};

glClearColor(flowLight[0], flowLight[1], flowLight[2], flowLight[4]);
glEnable(GL_FOG);
glFogfv(GL_FOG_COLOR, flowLight);
glFogf(GL_FOG_START, 1.0f);
glFogf(GL_FOG_END, 30.0f);
glFogi(GL_FOG_MODE, GL_LINEAR);

https://github.com/user-attachments/assets/12ac94aa-b275-468a-97a0-c95ba9ad83c6


# 누적 버퍼(accumulation buffer)
색상 버퍼를 렌더링하고 그 결과를 화면에 출력하지않고, 누적 버퍼에 복사한다.

누적된 여러 개의 색상 버퍼를 블렌딩해 결과를 만들어내는데,

많은 메모리 복사와 처리가 필요해 실시간으로는 사용하지않고, 실시간이 아닌 렌더링에 경우 사용한다.(효과가 아주 좋다!)


# 색상 관련 작업들

### Color Masking
최종 색상을 계산해 color buffer에 쓰였을때,
glColorMask 함수를 쓰면 하나 이상의 색상 채널에 대한 masking이 가능하다.

void glColorMask(GLboolean 적색, GLboolean 녹색, GLboolean 청색, GLboolean 알파);
각 인자에 GL_TRUE를 넣으면 해당 채널 쓰기를 허용하고,
GL_FALSE를 넣으면 쓰기를 차단한다.

이걸로 특정 색상 요소를 빼서 출력을 할 수 있다.

### 색상의 논리적(Logical) 연산
대부분 2D Graphic API에서는 source와 destination 색상 간의 바이너리 논리 연산이 가능하다.

void glLogicOp(GLenum 작업);
![image](https://github.com/user-attachments/assets/1ad5c30f-31d0-4b88-9ec7-d79fafdffa9b)


### Alpha Testing
특정 부분에 대해 알파 검사를 수행해, 통과하지 못하면 렌더링하지 않는 기능
void glAlphaFunc(GLenum 함수, GLclampf 참조);
![image](https://github.com/user-attachments/assets/3b05818a-f8b6-4d7b-b3a3-e3ef9182a431)



### Dithering (디더링)
: 몇 가지의 분리된 색상을 사용해서 훨씬 넓은 범위의 색상을 표현하는 기법을 의미함.
회색이 없더라도 검은색과 회색의 배치 만으로 회색을 표현할 수 있음.

glEnable(GL_DITHER);
높은 색상 해상도에서는 필요가 없고
8비트, 16비트 색상 지원 출력 시스템에서 유용하다!


