
# Part 1 Classic OpenGL
## Curve & Surface

![image](https://github.com/user-attachments/assets/7c5f0325-cd22-439e-9633-af81e3b98ee0)
### 이차 곡면
2차 곡면은 객체로 사용된다.

GLUQuadricObj 데이터 타입에 대한 포인트를 만들어서 사용.

GLUquadric *q = gluNewQuadric();   
gluQuadricDrawStyle(q,  …);        
gluSphere(q, …);                   
gluDeleteQuadric(q);               

![image](https://github.com/user-attachments/assets/395e6fc1-8ff2-46d3-a2e2-5cc7e65a0d6b)


# 베지어 곡선

![image](https://github.com/user-attachments/assets/91c8c211-70be-4e57-9a3c-78a097e47bfe)




곡선의 구간 내를 반복하면서 glEvalCoord를 호출하여 버텍스를 만들고 나면,
라인 스트립으로 곡선을 그릴 수 있다.


glMapGrid 함수를 이용하면 더 쉽게 그릴수도있음.

glEvalMesh를 호출하여 지정된 기본 모델로 점들을 연결하는 것!

glMapGrid1d(100,0.0, 100.0); //0부터 100까지의 범위 내에 100개의 점을 그리기

glEvalMesh1(GL_LINE, 0, 100);

### 3차원으로 늘리면
![image](https://github.com/user-attachments/assets/f69affe5-97ed-422e-9583-c945cf8d1e2a)

### 곡선생성기는 법선을 제공한다. -> 색상 계산!
![image](https://github.com/user-attachments/assets/7ed97f51-ed95-40fe-9915-75ebf1f08d95)


![image](https://github.com/user-attachments/assets/86f62d6e-681d-4e9f-a098-e8fa51371655)

# NURB
베지어 곡선보다 더 일반화된 방법.
베지어 곡선은 제어점이 늘어나면, 곡선을 유지하기가 어려워진다..

NURB (non-uniform rational B-spline)

### B-splines(bi-cubic splines)
베지어와 비슷하지만, 곡선을 여러부분으로 나누어서 가장 가까이 있는 네 개의 제어점의 영향만을 받는다.

### Knot
NURBS는 네 개의 제어점이 가진 영향력을 조절해 곡선 모양을 바꿀 수 있다.
knot이라는 값을 통해 이루어지며

모든 제어점에 대해서 2 개의 knot 값이 주어진다.

 ![image](https://github.com/user-attachments/assets/0672e3dc-58b8-4077-8196-f0ad914e4d96)



# 테설레이션
openGL의 모든 지오메트리를 볼록한 형태로 만들어야 빠르지만..

그렇게 하기 어렵다.
![image](https://github.com/user-attachments/assets/57a333a5-8d2c-40be-bf49-c64e3488a583)




![image](https://github.com/user-attachments/assets/b18a9f34-fe14-4b78-95e6-3e4620776a37)
복잡한 도형은 삼각형으로 나누어 표현하면 좋을 것이다.


이렇게 복잡한 형태의 폴리곤을 보다 제어가 쉽게 만드는행위를
### 테설레이션 tessellation이라고 한다.


