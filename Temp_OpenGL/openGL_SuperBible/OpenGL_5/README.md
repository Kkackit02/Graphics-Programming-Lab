# Part 1 Classic OpenGL
## Color, Materials, Lighting : The Basic


색상 얘기 블라블라..
1) 눈의 원리
2) CRT 원리
3) 색상 모드(4비트, 8비트, 16비트, 32비트) - 색상의 역사

멀티 미디어 공학에서 배운 내용들~~



## OpenGL에서의 색상
- R
- G
- B
- A
8비트씩 각각, 총 32비트


### 드로잉 색 결정

```c
void glColor<x><t>(R,G,B,A);
x -> 인자 수 결정
t -> 인자의 데이터타입
```


```cpp
glColcor3ub(0,255,128);
```

### 쉐이딩
glColor는 현재 드로잉의 색상을 지정하는것이다.
즉 이 명령 뒤에 드로잉되는 모든 버텍스에 적용되는 색상을 지정하는 것이다.

즉 삼각형(버텍스 3개)의 각 버텍스의 색상을 모두 다르게 지정하면?
-> 보간된 색상이 나타나게됨.

### Code 5-1
![image](https://github.com/user-attachments/assets/656fc98f-0c90-4cd4-bd31-810b5ac52238)


```cpp

    glShadeModel(GL_SMOOTH); // 색상 보간

    glBegin(GL_TRIANGLES);
    glColor3ub(255, 0, 0);     // 빨강
    glVertex3f(0.0f, 200.0f, 0.0f);
    glColor3ub(0, 255, 0);     // 초록
    glVertex3f(200.0f, -70.0f, 0.0f);
    glColor3ub(0, 0, 255);     // 파랑
    glVertex3f(-200.0f, -70.0f, 0.0f);
    glEnd();

```



# 실제 세계의 색상(Color in the Real World)

- Diffuse(분산광)
- Specular(반사광)
- ambient(주변광)

### 이 세 개의 RBGA Color 값을 합한 것이 그 지점의 색상!


# 실제 세계의 재질(Material in the Real World) 

### emission property
- ka(ambient에 대한 재질 속성)
- kd(diffuse에 대한 재질 속성)
- ks(specular에 대한 재질 속성)

위에서 나온 각 광에 대한 값에 이걸 곱해주면된다.

즉 I = ka * ia + ks * is + ka * ia


주변광 ambient는 단지 그냥 ka와 ia를 곱하면 끝이지만..

분산광과 반사광은
표면과 광원 사이의 가곧, 광원 거리, 기타요소 들도 정해져야한다.


즉 I = ka * ia + ks * is * max(0, n'h)^p + kd * id * max(0,n'l)

### 조명 추가하기
```cpp
glEnable(GL_LIGHTING)
```
활성화시 장면 내에 있는 각 버텍스의 색상을 결정할때, material 속성과 lighting 인자를 계산에 넣게 된다.

이러한 인자의 지정은 SetupRC 함수에서 이루어진다.

### 조명 모델 설정하기
```cpp
glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight); //Ambient를 사용하도록 설정
```

### 재질 속성 설정하기
2가지의 방법이 있다.
1) 첫 번째 방법 :  각 폴리곤, 폴리곤 셋에 대하여 glMaterial 함수 호출해주기
```cpp
Glfloat gray[] = {0.75f, 0.75f, 0.75f, 1.0f);
glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE , gray);

glBegin(GL_TRIANGLES);
glVertex3f(-15.0f, 0.0f, 30.0f);
glVertex3f(0.0f, 15.0f, 30.0f);
glVertex3f(0.0f, 0.0f, -56.0f);
glEnd();
```

glMaterialfv의 
- 첫 번째 인자는 어떤 면에 재질 속성을 설정할것인가이며,
- 두 번째 인자는 어떤 속성을 설정할 것인가
- 세 번째 인자는 RGBA값이 담긴 배열로 속성 설정

이 호출 이후 나오는 모든 기본 모델들은 이 설정의 영향을 받게된다.

2) 두 번째 방법 : 색상 트래킹(Color Tracking) 방식
   재질에 대해 glColor 호출해주기
```cpp

glEnable(GL_COLOR_MATERIAL); //색상 트래킹이 가능하도록 설정
glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // 폴리곤 앞면에 ambient, diffuse 적용 + glColor에서 설정한 값이 적용될 재질 인자 지정

glcolor3f(0.75f, 0.75f, 0.75f);

glbegin...
...
..


```


## 어떤 방향이 위쪽일까?

openGL에서는 광원 지정시, 광원의 방향을 정해주어야 한다.

모든 방향으로 뻗어나가는 광원도 있지만, 특정 뱡향으로 비춰지는 경우도 있기때문

폴리곤 표면에 대한 쉐이딩 효과 계산을 위해서는 openGL이 조명방향과 물체 방향을 기준으로 각도 계산을 해야한다.

![image](https://github.com/user-attachments/assets/2b18f749-13e9-4b59-99bc-1fa33c886960)

위 그림과 같이 빛과 눈 사이의 각도 계산을 통해 조명 계산을 한다!


폴리곤은 각 버텍스의 집합이며, 버텍스는 하나의 점이다.
문제는 버텍스는 점이고, 조명은 선이다.

점과 선 사이의 각도를 계산할 수 있는가?

불가능하다

그러므로 각 버텍스에 대한 조명 각도를 계산하기 위해서 각 버텍스에 대한 법선 벡터(normal Vector)를 활용해야한다.

법선 벡터는 버텍스 기준으로 임의의 평면이 있다고 했을때, 그 평면과 90도를 이루게 하면 된다.

![image](https://github.com/user-attachments/assets/00f4a2b9-8a2f-4c9a-b25c-72d4ed84ef82)

### 왜 모든 버텍스에 대해 법선을 만들어야할까?

폴리곤이 평면이 아닐수도있음..!
완벽한 조명을 위해서는 모든 버텍스에 법선 벡터가 지정이 되어있어야한다.

### 법선 벡터를 지정하기
![image](https://github.com/user-attachments/assets/3eb88c01-b38f-47fd-92dd-6f15b1f77cca)

다음과 같이 xz 평면 상의 한 면에서, (1,1,0)을 지나면서 이 면에 직각인 선을 기준으로

그 선 위에 있는(y축 기준으로) 점을 하나 정한뒤(1,1,10)
그 두 점을 이으면된다.
```cpp
glBegin(GL_TRIANGLES);
glNormal3f(0.0f, -1.0f, 0.0f);
glVertex3f(0.0f, 0.0f, 60.0f);
glVertex3f(-15.0f, 0.0f, 30.0f);
glVertex3f(15.0f,0.0f,30.0f);
glEnd();
```

glNormal3f 함수는 이 삼각형의 표면과 직각을 이루는 법선 벡터를 지정하는 것이다.
법선 벡터는 폴리곤의 앞면을 기준으로 뻗어나간다!

따라서 법선 벡터가 나오는 부분에서 폴리곤을 보면, 버텍스는 CCW로 지정되어있을 것이다.

### 단위 법선 벡터
openGL 에서는 법선 벡터를 처리하기 위해서, 법선 벡터로 바꾸는 과정을 거친다.

단위 법선(Unit Normal)이란 길이가 1인 법선 벡터를 말한다.(단위 자체가 길이가 1)

이를 위해 정규화(Normalization)을 거친다.
(벡터의 세 성분을 각각 제곱하고, 더한 값의 제곱근)

조명과 관련된 모든 법선 벡터는 무조건 무조건 무조건 정규화가 되어야한다.
```cpp
glEnable(GL_NORMALIZE);
```
이 방식은 성능저하가 일어나기때문에
가급적 직접 계산하는게 더 낫다!

### 법선 벡터 구하기

평면 폴리곤 세 지점 -> 법선 벡터
1) 세 지점을 통해 두 벡터를 만든다.
2) 두 벡터가 한 지점에서 서로 만나고 있다면..
3) 두 벡터의 외적을 통해 법선 벡터를 구할 수 있다.


법선 벡터는 미리 계산해두는 것이 효율적이다.

###
1) (Ambient , Diffuse)
   
https://github.com/user-attachments/assets/2603194a-e20f-4118-afd2-02c1c400610f


2) specular 추가
   
https://github.com/user-attachments/assets/f0c45a15-83be-4d53-91a0-c6871fbda5da



어라, specular가 이상하다!

###
구의 각 면에 대해 법선을 지정하면, 구는 거대한 보석처럼 될 것!
![image](https://github.com/user-attachments/assets/a5d24a2a-d7a1-41f8-ad3f-5b1bfa09c532)


사실 진짜 법선은 버텍스에 대한 법선이 표면의 진짜 법선일것이다.

구체는 tangent line에 대한 법선을 통해 구할 수 있다.
tangent line : 곡선의 한 점만 접하면서 뻗어나가고, 다른 부분과는 닿지않음.

![image](https://github.com/user-attachments/assets/f8ebd14c-04bd-4c35-8f42-8f1fd6974c56)

이를 통해 보다 부드러운 조명 계산이 가능하다.



### 다만....
구체만 편한거지, 복잡해지면 저렇게 못구한다!

따라서 버텍스를 공유하는 폴리곤들에 대해 각각 법선을 구하고, 그 평균을 내서 적용시킨다.
전과 비교해서 상당히 깔끔하고, 부드러운 조명 효과를 얻을 수 있다.

# 그림자

그림자를 만드는 알고리즘은 매우 다양하다.


지면에 그림자를 표현하는건

물체를 그냥 지면에 투영(projection)시키고 검은색으로 바꾼게 다임!!


### Z - Fighting(z-충돌)
![image](https://github.com/user-attachments/assets/760e95d4-f46b-48db-b0f1-2040d9e9962f)

두 물체가 같은 위치를 차지할때.,.,.,. 누구를 우선순위로 그려줘야하는가?

이럴땐 

1) 깊이 테스팅을 꺼준다.(바닥을 그릴때만!!)
2) 그림자를 약간 뛰워서 그린다.
3) depth buffer를 사용해서 우선순위값을 맥여준다.

![image](https://github.com/user-attachments/assets/495bb8f0-c19f-40e6-8a53-8235f9b472d0)


