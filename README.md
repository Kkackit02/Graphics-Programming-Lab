<div align="center">

# 🖥️ Graphics Programming Lab

<img src="01-Academic-Courses/3-2-Advanced-Computer-Graphics/ACG_HomeWork3/202112346%20정근녕%20제출(Assignment1)/Spline%20Image.png" width="800" alt="Graphics Lab Thumbnail"/>

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?style=flat-square&logo=opengl&logoColor=white)
![Vulkan](https://img.shields.io/badge/Vulkan-AC162C?style=flat-square&logo=vulkan&logoColor=white)
![GLSL](https://img.shields.io/badge/GLSL-4B8BBE?style=flat-square)
![GLFW](https://img.shields.io/badge/GLFW-3B6B3C?style=flat-square)

**소프트웨어 래스터라이징부터 Vulkan Ray Tracing까지 — 그래픽스 학습 아카이브**

</div>

---

## 📁 구조

```
Graphics-Programming-Lab/
├── 01-Academic-Courses/
│   ├── 3-1-Computer-Graphics/       # 컴퓨터 그래픽스 (차영운 교수)
│   └── 3-2-Advanced-Computer-Graphics/  # 고급 컴퓨터 그래픽스
├── 02-Self-Study-Log/               # 개인 학습
└── Temp_OpenGL/                     # 예제 & 연습
```

---

## 📚 01-Academic-Courses

### 🔵 3-1 Computer Graphics

> **소프트웨어 렌더러 직접 구현** — GPU 없이 C++로 래스터라이징 파이프라인 전 과정 구현

<table>
<thead>
<tr><th>과제</th><th>주제</th><th>핵심 구현</th></tr>
</thead>
<tbody>
<tr>
<td><b>Assignment 5</b></td>
<td>소프트웨어 래스터라이징</td>
<td>Barycentric 좌표 · Z-buffer · Triangle rasterization · 구(Sphere) 메시 생성</td>
</tr>
<tr>
<td><b>Assignment 6-1</b></td>
<td>Flat Shading</td>
<td>면 단위 법선 계산 · Blinn-Phong 조명 모델 · Diffuse / Specular</td>
</tr>
<tr>
<td><b>Assignment 6-2</b></td>
<td>Gouraud Shading</td>
<td>정점 단위 조명 계산 · 색상 선형 보간 · 재질(Material) 시스템</td>
</tr>
<tr>
<td><b>Assignment 8-Q1</b></td>
<td>GPU 메시 렌더링 & 성능 측정</td>
<td>OBJ 로딩 (Bunny) · OpenGL Timer Query · GLFW / GLEW</td>
</tr>
<tr>
<td><b>Assignment 8-Q2</b></td>
<td>GLSL 셰이더 파이프라인</td>
<td>Vertex / Fragment Shader 직접 작성 · VAO / VBO · 셰이더 메시 렌더링</td>
</tr>
</tbody>
</table>

#### 조명 모델 발전 과정

```
소프트웨어 래스터라이징 → Flat Shading → Gouraud Shading → GLSL Shader Pipeline
     (Assignment 5)         (6-1)           (6-2)              (Assignment 8)
```

---

### 🔴 3-2 Advanced Computer Graphics

> **Vulkan Ray Tracing + OpenGL 고급 기법** — 하드웨어 가속 광선 추적 및 애니메이션 시스템

<table>
<thead>
<tr><th>과제</th><th>주제</th><th>핵심 구현</th><th>결과물</th></tr>
</thead>
<tbody>
<tr>
<td><b>HomeWork 2</b></td>
<td>기하학 변환 & 장면 관리</td>
<td>GLM 행렬 변환 · OBJ 로딩 · Scene Graph · Geometry / Fragment Shader</td>
<td>-</td>
</tr>
<tr>
<td><b>HomeWork 3</b></td>
<td>스플라인 경로 애니메이션</td>
<td>Catmull-Rom Spline · Geometry Shader · 제어점 파일 로드 · Phong 조명</td>
<td><img src="01-Academic-Courses/3-2-Advanced-Computer-Graphics/ACG_HomeWork3/202112346%20정근녕%20제출(Assignment1)/Spline%20Image.png" width="200"/></td>
</tr>
<tr>
<td><b>Assignment 2</b></td>
<td>Vulkan Hardware Ray Tracing</td>
<td>VK_KHR_ray_tracing · SPIR-V 셰이더 · BLAS/TLAS · 실내 씬 (5개 오브젝트 + 광원)</td>
<td>-</td>
</tr>
</tbody>
</table>

#### Vulkan Ray Tracing 셰이더 구성 (Assignment 2)

| 셰이더 | 역할 |
|--------|------|
| `raygen.rgen` | 카메라로부터 광선 생성 |
| `closesthit.rchit` | 가장 가까운 교점 · 조명 계산 |
| `miss.rmiss` | 광선 미스 → 배경색 |
| `shadow.rmiss` | 그림자 레이 처리 |

---

## 📖 02-Self-Study-Log

### Vulkan API Practice
- **참고서**: *Vulkan Programming Guide* (Graham Sellers)
- Vulkan 인스턴스 생성, 메모리 할당, 렌더링 파이프라인 구성

---

## 🗂️ Temp_OpenGL

| 폴더 | 내용 |
|------|------|
| `konkukCG/CG_OpenGL/` | 건국대 그래픽스 수업 백업 (Assignment 01 ~ 08, Ray Casting 등) |
| `openGL_SuperBible/` | OpenGL SuperBible 챕터별 예제 (Chapter 2 ~ 10) |

---

## 🛠️ Tech Stack

| 분류 | 기술 |
|------|------|
| Language | C / C++17 |
| Graphics API | OpenGL 4.x · Vulkan 1.2 (KHR Ray Tracing) |
| Shader | GLSL · SPIR-V (`.rgen` `.rchit` `.rmiss`) |
| Math | GLM (행렬 · 벡터 · 쿼터니언) |
| Window / Input | GLFW · FreeGLUT |
| Extension Loader | GLEW |
| Model | OBJ (tiny_obj_loader) |
| IDE | Visual Studio 2022 |
