# Part 1 Classic OpenGL


# 3-1
  
  ![image](https://github.com/user-attachments/assets/ce53af0f-ba45-44ba-b5d6-4c6ae54ee67b)



# 3-3 

  [각 vertex의 크기도 다르게할수있음!]
  
  ![image](https://github.com/user-attachments/assets/443a6d98-1aa9-4f14-876a-a02c2ff49047)

   너무 큰 값은 알아서 최대 크기로 지정해준다.
   근데 만약 1픽셀보다 작아지면? -> 
   
   glPointSizr 내의 지정된 점의 크기는 픽셀의 정확한 크기가 아니라
픽셀 전체를 감싸는 원의 직경에 가장 가까운 값이라는걸 알아둬야함.


# 3-4

## GL_LINES 로 glBegin을 해서, 버텍스를 찍으면 선이 그어진다!
![image](https://github.com/user-attachments/assets/dd8ab971-99fe-4518-9934-10a8878937eb)


# 3-5
## GL_LINE_STRIP 은 선을 계속해서 이을 수 있다.

3-2 예시에서 GL_POINTS 대신 GL_LINE_STRIP을 넣은 결과
![image](https://github.com/user-attachments/assets/8154332b-c133-445c-a9c3-fad0d6b1b806)


# 3-6 
점 크기처럼, 점 두께도 설정할수있다.

void glLineWidth(GLfloat 두께);

점선도 가능(GL_LINE_STIPPLE) 등등..

![image](https://github.com/user-attachments/assets/ffd9a2ed-da85-42fe-94d0-86b15c71541a)





# 와인딩
폴리곤을 그릴때
버텍스를 잇는 선들은 방향이 정해져있다.
버텍스가 지정된 순서와 방향의 조합을 와인딩(Winding)이라고 한다.
OpenGL은 표준적으로 CCW(반시계) 와인딩을 따른다.
만약 이 표준을 바꾸고 싶다면
glFrontFace(GL_CW);

## 스트립(Strip)을 왜 할까?

삼각형을 그릴때 스트립핑을 해놓으면 삼각형 하나를 그리고, 다음 삼각형 하나를 그릴때, vertex가 하나만 필요하다!(2개의 vertex를 공유해서)

- GL_TRIANGLE_STRIP
- GL_TRIANGLE_FAN



그 외에도 GL_QUADS, GL_QUADSTRIP, GL_POLYGON!!


### 스티플링? 텍스쳐 매핑?

스티플링은 그냥 물체 위에 그대로 얹는 것(회전영향 X)

텍스처 매핑은 회전에도 다 영향을 받는다

## 폴리곤 구성 규칙
1) 모든 폴리곤은 하나의 평면에 있어야한다. 버텍스가 다른 평면에 있으면 뒤틀림..
2) 폴리곤을 이루는 선들이 서로 교차해서는 안되며 폴리곤의 전체적인 모양이 볼록해야한다.


이 외에도 스텐실과 버퍼들에 대한 내용들

버퍼 쪽은 나중에 한번 더 훑어볼만한듯!?
