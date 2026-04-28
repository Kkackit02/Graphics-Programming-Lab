# 고급컴퓨터그래픽스 통합 자료집 (상세판)

이 문서는 `D:\GitClone\AdvanceComputerGraphic\AdvanceCG\실습자료\강의자료` 디렉토리에 있는 PDF 강의자료들을 통합하고 상세하게 보강한 내용입니다.

---

## 목차

1.  [Physically Based Rendering (PBR)](#class-15-physically-based-rendering)
2.  [Curves I: 곡선의 표현](#acg_c05_curves_i)
3.  [Curves II: 스플라인 곡선, Hermite, Bezier 및 연속성](#acg_c06_curves_ii_22update)

5.  [Curves IV: 곡선과 쉐이더](#acg_c09_curves_iv)
6.  [Surfaces: 곡면](#acg_c11_surfaces)
7.  [Shader I & II & Review: 쉐이더 기법](#advanced-shaders-i-ii-review)
8.  [Image Processing: 쉐이더를 이용한 이미지 처리](#image-processing-with-shader)
9.  [Framebuffer Object (FBO): 프레임버퍼 객체](#acg_c14-2_fbo)
10. [Interpolation: 보간 기법](#interpolation)
11. [Animation: 애니메이션 기법](#animation)
12. [Dynamics: 동역학](#dynamics)
13. [Ray Tracing: 레이 트레이싱](#ray-tracing)

---

## Class 15 Physically Based Rendering

### PBR의 3대 핵심 개념
PBR은 빛의 물리적 동작을 시뮬레이션하여 사실적인 이미지를 구현하는 렌더링 패러다임입니다.

1.  **미세면 이론 (Microfacet Theory):**
    - 거시적으로는 매끄러워 보이는 표면도 미시적으로는 수많은 미세한 면(microfacet)으로 이루어져 있다고 가정합니다.
    - **Roughness (거칠기):** 이 미세면들의 법선(normal)이 얼마나 불규칙하게 분포하는지를 나타냅니다.
        - **Roughness ↓ (매끄러운 표면):** 미세면들이 한 방향으로 정렬되어 있어, 빛이 정반사되어 작고 선명한 하이라이트를 만듭니다.
        - **Roughness ↑ (거친 표면):** 미세면들이 무작위 방향으로 분포하여, 빛이 여러 방향으로 난반사되어 넓고 흐릿한 하이라이트를 만듭니다.

2.  **에너지 보존 (Energy Conservation):**
    - 표면에서 반사되는 빛의 총량은 표면에 닿는 빛의 총량을 초과할 수 없습니다.
    - 빛이 표면에 닿으면 일부는 반사(specular)되고 일부는 굴절(refraction)되어 내부로 흡수/산란(diffuse)됩니다. 반사되는 빛의 양이 많아지면, 흡수/산란되는 빛의 양은 줄어들어야 합니다.
    - 쉐이더 구현 시, diffuse와 specular의 합이 1을 넘지 않도록 조절해야 합니다.

3.  **반사율 함수 (BRDF - Bidirectional Reflectance Distribution Function):**
    - 들어오는 빛(light)의 방향 `l`과 나가는 빛(view)의 방향 `v`에 대해, 표면이 빛을 얼마나 반사하는지를 나타내는 함수 `f(l, v)` 입니다.
    - PBR의 BRDF는 주로 **Diffuse(난반사)** 항과 **Specular(정반사)** 항의 합으로 구성됩니다.
    - `f_r = k_d * f_lambert + k_s * f_cook-torrance`
        - `k_d`: 난반사 비율
        - `k_s`: 정반사 비율 (`k_d + k_s = 1.0`)

### Cook-Torrance Specular BRDF
`f_cook-torrance = D * F * G / (4 * (n·v) * (n·l))`
- **D (Normal Distribution Function):** 미세면 법선 분포 함수. 특정 방향을 향하는 미세면의 비율을 나타냅니다. (Trowbridge-Reitz GGX가 널리 사용됨)
- **F (Fresnel Equation):** 프레넬 방정식. 시야각에 따라 표면의 반사율이 어떻게 변하는지를 설명합니다. (Schlick's Approximation이 널리 사용됨)
    - `F_schlick = F0 + (1 - F0) * (1 - (h·v))^5`
    - `F0`: 표면을 수직으로 바라봤을 때의 반사율. 금속은 Albedo 값을, 비금속은 작은 상수(약 0.04)를 사용합니다.
- **G (Geometry Function):** 기하학 함수. 미세면들끼리 서로 가리는 현상(shadowing/masking)을 모델링합니다. (Smith's Method가 GGX와 함께 사용됨)

### 렌더링 방정식 (The Render Equation)
`Lo(p, wo) = Le(p, wo) + ∫Ω fr(p, wi, wo) Li(p, wi) (n · wi) dwi`
- `Lo`: 지점 `p`에서 `wo` 방향으로 나가는 최종 빛의 양 (Radiance)
- `Le`: 지점 `p`에서 `wo` 방향으로 스스로 방출하는 빛 (Emitted)
- `∫Ω`: 반구(hemisphere) `Ω` 내의 모든 방향에 대한 적분
- `fr`: BRDF
- `Li`: 지점 `p`로 `wi` 방향에서 들어오는 빛의 양
- `(n · wi)`: 입사각에 따른 빛의 감쇠(attenuation)

---

## ACG_C05_Curves_I: 곡선의 표현과 래스터화

### 1. 곡선의 표현 방법

컴퓨터 그래픽스에서 곡선을 표현하는 세 가지 주요 방식이 있습니다.

- **Explicit (명시적): `y = f(x)`**
    - **장점:** 특정 `x`에 대한 `y` 값을 쉽게 계산할 수 있습니다.
    - **단점:**
        - 하나의 `x`에 여러 `y` 값이 대응되는 수직선이나 원 같은 곡선은 표현이 불가능합니다.
        - 축에 종속적이라 회전 변환 등이 복잡합니다.

- **Implicit (음함수): `f(x, y) = 0`**
    - **예시:** 원의 방정식 `(x-xc)² + (y-yc)² - r² = 0`
    - **장점:** 특정 점 `(x, y)`가 곡선 위에 있는지, 내부에 있는지, 외부에 있는지 판별하기 매우 쉽습니다. (f=0: 위, f<0: 안, f>0: 밖)
    - **단점:** 곡선 위의 점들을 순서대로 생성하기 어려워 렌더링에 직접 사용하기는 까다롭습니다.

- **Parametric (매개변수): `P(t) = (x(t), y(t))`**
    - **예시:** 원의 방정식 `x(t) = xc + r*cos(t)`, `y(t) = yc + r*sin(t)`
    - **장점:**
        - `t`라는 단일 매개변수를 `0`에서 `1`까지 (또는 특정 범위까지) 증가시키며 곡선 위의 점들을 순서대로 쉽게 생성할 수 있습니다.
        - 접선(tangent) `P'(t)` 등 기하학적 속성을 계산하기 용이합니다.
        - 다차원(3D)으로 확장이 간편합니다.
    - **결론:** 이런 장점들 때문에 컴퓨터 그래픽스에서 가장 널리 사용되는 방식입니다.

### 2. 원(Circle) 래스터화: Midpoint Circle Algorithm

단순히 원의 방정식을 이용하여 픽셀을 찍으면 제곱근 계산으로 비용이 높고, `x`를 1씩 증가시킬 때 `y`의 변화량이 일정하지 않아 균일한 모양이 나오지 않습니다. 이를 해결하기 위해 정수 연산만을 사용하는 **Midpoint Circle Algorithm**이 사용됩니다.

- **핵심 아이디어:** 현재 픽셀 `(xk, yk)`가 정해졌을 때, 다음 `x` 위치(`xk+1`)에서 선택될 픽셀은 `(xk+1, yk)` 또는 `(xk+1, yk-1)` 둘 중 하나입니다. 이 둘의 중간점(midpoint) `(xk+1, yk-0.5)`가 원의 안쪽에 있는지 바깥쪽에 있는지를 판별하여 다음 픽셀을 결정합니다.

- **판별식 (Decision Variable) `pk`:**
    - 원의 음함수 `f(x, y) = x² + y² - r²` (중심이 원점일 때)를 이용합니다.
    - `pk = f(xk+1, yk-0.5) = (xk+1)² + (yk-0.5)² - r²`
    - **`pk < 0`**: 중간점이 원 안쪽에 있음 -> 다음 픽셀은 위쪽인 `(xk+1, yk)` 선택.
    - **`pk >= 0`**: 중간점이 원 바깥쪽에 있음 -> 다음 픽셀은 아래쪽인 `(xk+1, yk-1)` 선택.

- **`pk`의 점진적 계산 (Incremental Calculation):**
    - 매번 `pk`를 새로 계산하는 것은 비효율적입니다. `pk+1`을 `pk`를 이용하여 계산합니다.
    - `pk+1 = pk + 2(xk+1) + (yk+1² - yk²) - (yk+1 - yk) + 1`
    - **if `pk < 0` (yk+1 = yk):** `pk+1 = pk + 2xk+1 + 1`
    - **if `pk >= 0` (yk+1 = yk-1):** `pk+1 = pk + 2xk+1 + 1 - 2yk+1`
    - **초기값:** 시작점 `(0, r)`에서 `p0 = f(1, r-0.5) = 5/4 - r`. 정수 연산을 위해 `p0 = 1 - r`로 계산을 시작할 수 있습니다.

- **알고리즘 요약:**
    1. 시작점 `(0, r)`과 초기 판별식 `p0`를 계산합니다.
    2. `x`가 `y`보다 작거나 같을 때까지 다음을 반복합니다 (원의 1/8만 계산).
    3. `x`를 1 증가시키고, `p` 값에 따라 `y` 값을 유지하거나 1 감소시킵니다.
    4. `p` 값을 점진적으로 업데이트합니다.
    5. 계산된 점 `(x, y)`를 8방향 대칭(Symmetry)을 이용하여 나머지 7개의 점을 함께 찍습니다.

### 3. 타원(Ellipse) 래스터화
- 원과 유사하게 Midpoint Algorithm을 적용할 수 있습니다.
- 단, 타원은 기울기가 1이 되는 지점을 기준으로 두 개의 영역(Region 1, Region 2)으로 나뉩니다.
    - **Region 1:** `x`를 1씩 증가시키며 `y`를 결정.
    - **Region 2:** `y`를 1씩 증가시키며 `x`를 결정.
- 4방향 대칭을 이용하여 전체 타원을 그립니다.

---

---

## ACG_C06_Curves_II_22update: 스플라인 곡선, Hermite, Bezier 및 연속성

### 고급 2D 프리미티브 (곡선)
*   **곡선 표현:** 함수 (Functions)
    *   `y = Σ a_k x^k`, `y = e^x`, `y = log x`, `y = sin x` 등
*   **방정식 참조:**
    *   명시적 (Explicit): `y = f(x)`
    *   음함수 (Implicit): `f(x, y) = 0`
    *   매개변수 (Parametric): `x = f(u), y = g(u)`
*   **매개변수 곡선:**
    *   왜 사용하는가? 곡선 제어 (속도, 웨이포인트 등)
    *   `f(u) = (x(u), y(u))`
    *   `x(u) = Σ a_k u^k`, `y(u) = Σ b_k u^k`
*   **스플라인 곡선:**
    *   곡선 생성, 스트립 및 가중치, 제어점
    *   곡선의 수학적 모델링, 곡선 연결

### 보간 (Interpolation) 및 근사 (Approximation)
*   **곡선 사양:** 보간, 근사
*   **제어점**

### 스플라인 사양 (Spline Specification)
*   **지정 방법:**
    *   3차 스플라인: `x(u) = a_x u^3 + b_x u^2 + c_x u + d_x` 등
    *   2차 스플라인
*   **방정식 해결 방법:** 4개의 미지수
*   **경계 조건 (Boundary condition):**
    *   시작점, 끝점 지정
    *   시작점, 끝점의 1차 미분값 지정
    *   `x(u)`의 계수 `a_x, b_x, c_x, d_x`를 `x(0), x(1), x'(0), x'(1)`로 표현하는 공식
    *   `x(u) = U * M_spline * M_geom` 형태의 행렬 표현
    *   Hermite 기저 함수를 이용한 표현
*   **그리기 방법:** `u`를 `[0, 1]` 범위에서 반복하며 `(x(u), y(u))` 점을 그림. 각 세그먼트에 대해 반복.

### 곡선 및 연속성 (Curves & Continuity)
*   **연속 곡선:** 곡선 점들이 어떻게 만나는지에 따라 결정. 특히 두 세그먼트가 만나는 곡선 점에서 중요.
*   **연속성 (Continuity):**
    *   **C0 연속성 (Positional Continuity):** 두 점이 만남. `x1(1) = x2(0)`
    *   **C1 연속성 (Tangential Continuity):** 두 점이 부드럽게 만남. 1차 미분값이 같음. `x1(1) = x2(0)` 및 `x1'(1) = x2'(0)`
    *   **C2 연속성 (Curvature Continuity):** 두 점이 매우 부드럽게 연결됨. 2차 미분값이 같음. `x1(1) = x2(0)`, `x1'(1) = x2'(0)`, `x1''(1) = x2''(0)`
*   **연속성이 중요한 이유:** 물리적 속성, 움직임, CAD에서 곡선(곡면)의 연속성 중요.

### 기하학적 연속성 (Geometric Continuity)
*   **G0 연속성:** C0 연속성과 동일.
*   **G1 연속성:** `x1'(1) = α x2'(0)` (1차 미분 벡터가 같은 방향을 가짐, 크기는 다를 수 있음)
*   **G2 연속성:** `x1''(1) = α x2''(0)` (2차 미분 벡터가 같은 방향을 가짐, 크기는 다를 수 있음)

---

### 1. Hermite 곡선

- **개념:** 곡선의 **양 끝점**과 양 끝점에서의 **접선 벡터(tangent vector)**, 총 4개의 기하학적 정보로 곡선을 정의합니다.
- **기하 벡터 (Geometry Vector):** `G_h = [P_0, P_1, T_0, T_1]^T`
    - `P_0`: 시작점
    - `P_1`: 끝점
    - `T_0`: 시작점에서의 접선 벡터 `P'(0)`
    - `T_1`: 끝점에서의 접선 벡터 `P'(1)`
- **방정식:** `P(t) = (2t³-3t²+1)P_0 + (-2t³+3t²)P_1 + (t³-2t²+t)T_0 + (t³-t²)T_1`
    - 각 항의 다항식은 **블렌딩 함수(Blending Function)** 또는 **기저 함수(Basis Function)**라 불립니다.
    - 예를 들어, `t=0`일 때 `P(0) = P_0`가 되고, `t=1`일 때 `P(1) = P_1`이 되도록 함수가 설계되었습니다.
- **장점:**
    - 접선 벡터를 직접 제어하므로, 여러 곡선 세그먼트를 연결할 때 **C1 연속성**을 보장하기 매우 쉽습니다. (앞 곡선의 `P_1`, `T_1`을 뒷 곡선의 `P_0`, `T_0`로 사용)
- **단점:**
    - 접선 벡터의 크기와 방향을 직접 수치로 제어하는 것이 비직관적일 수 있습니다.
*   **ACG_C07_Curves_III 보강 내용:**
    *   **강점:** 제어 용이성 (Easy to control)
    *   **문제점:** 복잡한 형태에 대한 제한적인 제어 (Limited control on complex shapes), 연속성 문제 (Continuity issues) - *ACG_C06에서 C1 연속성 보장이 쉽다고 언급되었으나, ACG_C07에서는 '복잡한 상황에서의 연속성 문제' 또는 '직관적이지 않은 제어로 인한 문제'로 해석될 수 있음.*

### 2. Bezier 곡선

- **개념:** 4개의 **제어점(Control Points)** `P_0, P_1, P_2, P_3`를 이용하여 곡선을 정의합니다. 곡선은 양 끝점(`P_0`, `P_3`)만 통과하고, 중간 제어점(`P_1`, `P_2`)은 곡선의 형태를 조절하는 "자석"처럼 작용합니다.
- **기하 벡터 (Geometry Vector):** `G_b = [P_0, P_1, P_2, P_3]^T`
- **특징:**
    - **직관적인 제어:** 제어점을 마우스로 끌어 옮기면 곡선의 모양이 예측 가능하게 부드럽게 변형되어 디자이너에게 친숙합니다.
    - **Convex Hull Property:** 곡선은 항상 모든 제어점을 포함하는 가장 작은 볼록 다각형(Convex Hull) 내부에 존재합니다. 이로 인해 렌더링 및 충돌 검사 시 계산을 최적화할 수 있습니다.
    - **접선 관계:** 시작점에서의 접선은 `P_0`에서 `P_1`을 향하고, 끝점에서의 접선은 `P_2`에서 `P_3`를 향합니다. (`P'(0) = 3(P_1-P_0)`, `P'(1) = 3(P_3-P_2)`)
*   **ACG_C07_Curves_III 보강 내용:**
    *   **시작:** Pierre Bézier (르노)
    *   **근사 곡선 (Approximation curve)**
    *   **정의:** `n+1`개의 제어점 `P_k`에 대해 `P(u) = Σ P_k B_{k,n}(u)`
        *   `B_{k,n}(u) = C(n,k)u^k (1-u)^{n-k}` (Bernstein 다항식)
    *   **속성:**
        *   **시작 및 끝점 조건:** `P(0) = P_0`, `P(1) = P_n`
        *   **볼록 껍질 속성 (Convex hull property):** 곡선은 항상 제어점들의 볼록 껍질 내부에 존재.
            *   `Σ B_{k,n}(u) = 1` (Partition of Unity)
            *   선형 조합 (Linear Combination), 아핀 조합 (Affine Combination), 볼록 조합 (Convex Combination)
    *   **그리기:**
        *   `u` 값을 `0`에서 `1`까지 증가시키며 각 `u`에 대해 `P(u) = Σ B_{k,n}(u) * P_k`를 계산하여 점을 찍음.
    *   **사용 예시:**
        *   닫힌 곡선 (Closed curve)
        *   가중치 곡선 (Weighted curve)
        *   연결된 곡선 (Connected curve)
        *   고차 곡선 (Higher order curve)
    *   **연속성 보장 (C1):** 연결된 곡선에서 C1 연속성을 보장하는 방법 (접선 벡터 관계식 제시)
    *   **De Casteljau 알고리즘:**
        *   재귀적인 선형 보간을 통해 곡선 위의 점을 찾는 기하학적 방법.
        *   `p_i^j = (1-t)p_i^{j-1} + t p_{i+1}^{j-1}`
        *   `P(t) = p_0^n`

#### a) Bernstein 다항식을 이용한 정의 (대수적 접근)
- **방정식:** `P(t) = Σ_{i=0}^{3} P_i * B_{i,3}(t)`
- **Bernstein Basis Function:** `B_{i,n}(t) = C(n, i) * t^i * (1-t)^{n-i}`
    - `B_{0,3}(t) = (1-t)³`
    - `B_{1,3}(t) = 3t(1-t)²`
    - `B_{2,3}(t) = 3t²(1-t)`
    - `B_{3,3}(t) = t³`
- **기저 함수의 속성:**
    - **Partition of Unity:** 모든 `t`에 대해 기저 함수의 합은 항상 1입니다. (`Σ B_{i,n}(t) = 1`)
    - **Positivity:** `0 < t < 1` 범위에서 모든 기저 함수는 양수입니다.

#### b) de Casteljau 알고리즘 (기하학적 접근)
- 재귀적인 선형 보간(Linear Interpolation, Lerp)을 통해 곡선 위의 점을 찾는 기하학적인 방법입니다.
- **과정 (3차 베지에의 경우):**
    1. 제어점 `P_0, P_1, P_2, P_3`가 주어집니다.
    2. `t`에 대해 선분 `P_0P_1`, `P_1P_2`, `P_2P_3`를 각각 선형 보간하여 점 `P_0_1`, `P_1_1`, `P_2_1`을 찾습니다.
       - `P_0_1 = (1-t)P_0 + tP_1`
    3. 다시 선분 `P_0_1 P_1_1`, `P_1_1 P_2_1`를 선형 보간하여 점 `P_0_2`, `P_1_2`를 찾습니다.
    4. 마지막으로 선분 `P_0_2 P_1_2`를 선형 보간한 점 `P_0_3`가 바로 `t`에서의 곡선 위의 점 `P(t)`입니다.

### 3. 곡선 세그먼트 연결하기
두 개의 3차 베지에 곡선 세그먼트 `Q(t)` (제어점 Q0, Q1, Q2, Q3)와 `R(t)` (제어점 R0, R1, R2, R3)를 부드럽게 연결하려면:

- **C0 연속성:** `Q3 = R0`. 첫 번째 곡선의 끝점과 두 번째 곡선의 시작점을 일치시킵니다.
- **C1 연속성:** C0 조건을 만족하면서, 연결점에서의 접선이 동일한 방향을 가져야 합니다.
    - `Q2`, `Q3(=R0)`, `R1` 세 점이 하나의 직선 위에 있어야 합니다.
    - 더 엄밀한 G1 연속성을 위해서는 `Q3 - Q2 = R1 - R0` (접선 벡터의 크기와 방향이 모두 같음) 조건을 만족해야 합니다.

### 4. Bezier 곡선의 강점 및 제약 (ACG_C07_Curves_III 보강 내용)
*   **강점:**
    *   C1 연속 곡선을 쉽게 생성 가능 (Could generate C1 continuous curve easily)
    *   곡선 제어가 비교적 용이 (Be able to control curves relatively easy)
*   **제약:**
    *   많은 제어점을 사용할 경우 곡선 차수가 너무 높아짐 (Curve degree would get too high to give many control points)

---

## ACG_C07_Curves_III: B-Spline 곡선

B-Spline(Basis Spline)은 베지에 곡선을 일반화한 형태로, 복잡한 곡선을 모델링할 때 발생하는 문제들을 해결하기 위해 등장했습니다.

### 1. B-Spline의 필요성: 베지에 곡선의 한계

- **차수 문제:** 베지에 곡선은 제어점의 개수가 늘어나면 곡선의 차수(degree)도 함께 높아집니다. 차수가 높아지면 계산이 복잡해지고, 곡선 전체가 예상치 못하게 크게 출렁이는 현상이 발생할 수 있습니다.
- **국부 제어(Local Control)의 부재:** 베지에 곡선은 제어점 하나만 움직여도 곡선 전체의 모양이 변경됩니다. 이는 세밀한 수정 작업을 어렵게 만듭니다.

B-Spline은 **곡선의 차수를 제어점 개수와 무관하게** 설정하고, **국부 제어**를 가능하게 하여 이러한 문제들을 해결합니다.

### 2. B-Spline의 정의

- **방정식:** `P(t) = Σ_{i=0}^{n} P_i * N_{i,k}(t)`
    - `P_i`: 제어점 (n+1개)
    - `k`: 곡선의 차수(degree). `k-1`차 곡선을 의미하기도 함 (order = k).
    - `N_{i,k}(t)`: `i`번째 제어점에 대한 `k`차 B-Spline **기저 함수(Basis Function)**.

- **핵심 특징:**
    - **Local Control:** 기저 함수 `N_{i,k}(t)`는 특정 구간 `[t_i, t_{i+k}]`에서만 0이 아닌 값을 가집니다. 따라서 제어점 `P_i`는 해당 구간의 곡선에만 영향을 미칩니다.
    - **C^(k-2) 연속성:** 별다른 처리 없이도 매듭(knot) 지점에서 높은 수준의 연속성을 기본적으로 보장합니다.

### 3. B-Spline 기저 함수: Cox-de Boor 재귀 공식

B-Spline 기저 함수는 **매듭 벡터(Knot Vector)**에 의해 정의되며, 재귀적으로 계산됩니다.

- **Cox-de Boor Formula:**
    - **k=1 (0차):**
      `N_{i,1}(t) = 1`  (if `t_i ≤ t < t_{i+1}`)
      `N_{i,1}(t) = 0`  (otherwise)
      (매듭 구간 `[t_i, t_{i+1})`에서만 1의 값을 갖는 계단 함수)

    - **k > 1:**
      `N_{i,k}(t) = w(t) * N_{i,k-1}(t) + (1-w(t)) * N_{i+1,k-1}(t)`
      - `w(t) = (t - t_i) / (t_{i+k-1} - t_i)`
      - `1-w(t) = (t_{i+k} - t) / (t_{i+k} - t_{i+1})`
      (두 개의 `k-1`차 기저 함수를 선형 보간하여 `k`차 기저 함수를 만듭니다.)

### 4. 매듭 벡터 (Knot Vector)

- **정의:** `T = {t_0, t_1, ..., t_m}`. 매개변수 `t`의 분할 지점을 정의하는 값들의 시퀀스입니다. 기저 함수의 모양과 곡선의 형태를 결정하는 핵심 요소입니다.
- **규칙:** `t_i ≤ t_{i+1}` (증가하거나 같은 값을 가짐)
- **관계식:** `m = n + k` (제어점 개수 `n+1`, 차수 `k-1`일 경우 `m = n + k`)
- **종류:**
    - **Uniform Knot Vector:** 매듭 간격이 `[0, 1, 2, 3, ...]`처럼 균일합니다.
        - 곡선이 제어점을 통과하지 않으며, 주로 이론적인 설명에 사용됩니다.
    - **Open-Uniform Knot Vector:** 양 끝에 `k`개의 동일한 매듭 값을 가집니다.
        - **예시 (n=5, k=3):** `T = {0, 0, 0, 1, 2, 3, 3, 3}`
        - **특징:** 곡선이 양 끝 제어점(`P_0`, `P_n`)에서 시작하고 끝납니다. 베지에 곡선과 유사한 특징을 가지게 되어 모델링에 매우 유용합니다.
    - **Non-Uniform Knot Vector:** 매듭 간격이 불규일합니다. 특정 구간의 매듭을 중첩시켜 곡선의 모양을 더 세밀하게 제어할 수 있습니다.

- **매듭 다중도(Multiplicity)와 연속성:**
    - 특정 매듭 값 `t_i`가 `s`번 중복되면, 해당 지점에서 곡선의 연속성은 `C^(k-s-1)`로 감소합니다.
    - **예시 (k=4, 3차 곡선):**
        - 기본 연속성은 `C²` 입니다.
        - 특정 매듭이 2번 중복되면(`s=2`), 연속성은 `C¹`로 감소합니다. (뾰족해지기 시작)
        - 특정 매듭이 3번 중복되면(`s=3`), 연속성은 `C⁰`로 감소합니다. (완전히 꺾임)
        - 특정 매듭이 4번 중복되면(`s=4`), 곡선이 분리됩니다.

---

## ACG_C09_Curves_IV: NURBS 곡선

NURBS(Non-Uniform Rational B-Spline)는 B-Spline을 더욱 일반화한 형태로, 현재 컴퓨터 그래픽스와 CAD 분야의 표준적인 곡선/곡면 표현 방식입니다.

### 1. NURBS의 정의: Rational의 의미

- **Rational (유리식):** B-Spline의 정의에 **가중치(weight)**와 **나눗셈**이 추가된 것을 의미합니다.
- **방정식:**
  `P(t) = (Σ_{i=0}^{n} w_i * P_i * N_{i,k}(t)) / (Σ_{i=0}^{n} w_i * N_{i,k}(t))`
  - `P_i`: 3D 공간의 제어점 `(x, y, z)`
  - `w_i`: 각 제어점에 해당하는 스칼라 가중치
  - `N_{i,k}(t)`: B-Spline 기저 함수

- **B-Spline과의 관계:** 모든 가중치 `w_i`가 1이면, 분모는 B-Spline 기저 함수의 합(1)이 되므로 NURBS 곡선은 B-Spline 곡선과 동일해집니다. 즉, B-Spline은 NURBS의 특별한 경우입니다.

### 2. 동차 좌표계 (Homogeneous Coordinates)를 이용한 표현

NURBS의 유리식(나눗셈)은 계산을 복잡하게 만듭니다. 이를 해결하기 위해 **동차 좌표계**를 사용합니다.

1.  **4D로 확장:** 3D 제어점 `P_i = (x_i, y_i, z_i)`를 가중치 `w_i`를 사용하여 4D 동차 좌표 `P_i^w = (w_i*x_i, w_i*y_i, w_i*z_i, w_i)`로 변환합니다.
2.  **4D B-Spline 생성:** 이 4D 제어점들을 이용하여 표준적인 (Non-rational) B-Spline 곡선 `P^w(t)`를 생성합니다.
    `P^w(t) = Σ_{i=0}^{n} P_i^w * N_{i,k}(t)`
3.  **3D로 투영:** 계산된 4D 곡선 위의 점 `P^w(t) = (x, y, z, w)`를 마지막 `w` 컴포넌트로 나누어 다시 3D 공간으로 투영합니다.
    `P(t) = (x/w, y/w, z/w)`

- **장점:** 이 방식을 사용하면 복잡한 유리식을 직접 다루는 대신, 한 차원 높은 공간에서 표준 B-Spline 연산을 그대로 사용할 수 있어 구현과 계산이 훨씬 간단해집니다. 모든 변환(이동, 회전, 크기, 원근 투영)을 단일 행렬 곱으로 처리할 수 있게 됩니다.

### 3. 가중치(Weight)의 역할

- 가중치 `w_i`는 해당 제어점 `P_i`가 곡선을 얼마나 강하게 끌어당기는지를 결정합니다.
    - **`w_i` > 1:** 곡선이 `P_i`에 더 가깝게 끌려옵니다.
    - **`w_i` = 1:** 표준 B-Spline과 동일한 영향을 줍니다.
    - **0 ≤ `w_i` < 1:** `P_i`의 영향력이 줄어들어 곡선이 제어점에서 더 멀어집니다.
    - **`w_i` = 0:** 해당 제어점은 곡선에 아무런 영향을 주지 않습니다.
    - **`w_i` < 0:** 거의 사용되지 않으며, 곡선이 제어점에서 멀리 벗어나는 등 예측하기 어려운 형태가 될 수 있습니다.

### 4. NURBS의 장점: 원뿔 곡선(Conic Sections)의 정확한 표현

- B-Spline을 포함한 일반적인 다항식 곡선은 원, 타원, 포물선, 쌍곡선과 같은 원뿔 곡선을 '근사'할 수는 있지만 '완벽하게' 표현하지는 못합니다.
- **NURBS는 가중치를 조절하여 이러한 원뿔 곡선들을 수학적으로 정확하게 표현할 수 있습니다.**
- 예를 들어, 3개의 제어점과 `k=3` 차수, 매듭 벡터 `T={0,0,0,1,1,1}`을 갖는 2차 NURBS 곡선에서, 양 끝 제어점의 가중치를 1로, 가운데 제어점의 가중치를 조절함으로써 원호를 정확하게 만들 수 있습니다.

이러한 강력한 표현력 때문에 NURBS는 정밀한 모델링이 필수적인 CAD, CAM, 산업 디자인 분야의 표준으로 자리 잡았습니다.

---

## ACG_C11_Surfaces: 곡면

곡면은 3D 모델링에서 복잡한 형태를 표현하는 데 필수적인 요소입니다. 곡선과 마찬가지로 매개변수(Parametric) 방식을 주로 사용합니다.

### 1. 매개변수 곡면 (Parametric Surfaces)

- **정의:** `S(u, v) = (x(u, v), y(u, v), z(u, v))`
    - 두 개의 매개변수 `u`와 `v` (`0 ≤ u, v ≤ 1`)를 사용하여 3차원 공간상의 점을 정의합니다.
    - `u` 또는 `v` 중 하나를 고정하면 곡면 위에 놓인 곡선(isoparametric curve)이 생성됩니다.

- **곡면의 법선 벡터 (Normal Vector):**
    - 조명 계산에 필수적인 정보입니다.
    - `u` 방향의 편미분 벡터 `∂S/∂u`와 `v` 방향의 편미분 벡터 `∂S/∂v`는 곡면의 접선 벡터를 나타냅니다.
    - 이 두 접선 벡터의 외적(cross product)을 통해 곡면에 수직인 법선 벡터를 계산할 수 있습니다.
      `N(u, v) = (∂S/∂u) × (∂S/∂v)`

### 2. Tensor Product Surfaces (텐서 곱 곡면)

- 곡면을 생성하는 가장 일반적인 방법으로, 1차원 곡선 정의를 2차원으로 확장합니다.
- `S(u, v) = Σ_i Σ_j P_{i,j} * B_i(u) * B_j(v)`
    - `P_{i,j}`: 제어점 그리드 (Control Point Grid)
    - `B_i(u)`, `B_j(v)`: `u`와 `v` 방향의 기저 함수 (예: Bernstein 다항식, B-Spline 기저 함수)

#### a) Bilinear Surface (쌍선형 곡면)
- 4개의 제어점 `P_00, P_10, P_01, P_11`을 이용하여 정의되는 가장 간단한 곡면입니다.
- `S(u, v) = (1-u)(1-v)P_00 + u(1-v)P_10 + (1-u)vP_01 + uvP_11`
- `u`와 `v` 방향으로 각각 선형 보간을 두 번 수행하여 곡면을 생성합니다.

#### b) Bezier Surface (베지에 곡면)
- 베지에 곡선을 2차원으로 확장한 개념입니다. `(n+1) x (m+1)`개의 제어점 그리드를 사용합니다.
- `S(u, v) = Σ_{i=0}^{n} Σ_{j=0}^{m} P_{i,j} * B_{i,n}(u) * B_{j,m}(v)`
- **특징:**
    - **Convex Hull Property:** 곡면은 항상 제어점 그리드를 포함하는 볼록 껍질 내부에 존재합니다.
    - 곡면은 4개의 모서리 제어점만 통과합니다.
    - 제어점 그리드의 경계선은 베지에 곡선입니다.
- **Cubic Bezier Patch (3차 베지에 패치):** `4x4` 제어점 그리드를 사용합니다.

#### c) B-Spline / NURBS Surface
- B-Spline 곡선 또는 NURBS 곡선을 2차원으로 확장한 것입니다.
- **NURBS Surface:** 가장 일반적이고 강력한 곡면 표현 방법으로, CAD/CAM 분야의 표준입니다. 가중치를 통해 원뿔 곡선 등을 정확하게 표현할 수 있습니다.

### 3. 곡면 패치 연결 (Continuity for Surfaces)

- 여러 곡면 패치를 연결하여 복잡한 모델을 만듭니다.
- **C0 (Positional Continuity):** 두 패치의 경계선이 일치해야 합니다.
- **C1 (Tangential Continuity):** C0를 만족하고, 경계선에서 두 패치의 접평면(tangent plane)이 일치해야 합니다. 이는 경계선에서 `u`와 `v` 방향의 편미분 벡터가 선형적으로 종속되어야 함을 의미합니다.

### 4. Subdivision Surfaces (세분화 곡면)

- 다각형 메쉬(Polygon Mesh)를 반복적으로 세분화(subdivide)하여 부드러운 곡면을 생성하는 기법입니다.
- **장점:**
    - 적은 수의 제어점(초기 메쉬의 정점)으로 복잡하고 부드러운 곡면을 표현할 수 있습니다.
    - 토폴로지(Topology)에 제약이 적어 임의의 다각형 메쉬에서 시작할 수 있습니다.
    - GPU 구현이 용이합니다.
- **단점:**
    - '특별점(extraordinary point)' 주변에서 연속성 문제가 발생할 수 있습니다.
    - 경계면 제어가 복잡할 수 있습니다.

#### a) 주요 세분화 방법
- **Catmull-Clark Subdivision (캣멀-클라크 세분화):**
    - 쿼드(quad, 사각형) 메쉬에 적용되며, 결과적으로 쿼드 메쉬를 생성합니다.
    - 임의의 다각형 메쉬를 입력으로 받아도 최종적으로는 쿼드 메쉬로 수렴합니다.
    - `C²` 연속성을 보장하지만, 특별점 주변에서는 `C¹` 연속성을 가집니다.
    - 애니메이션 캐릭터 모델링에 널리 사용됩니다.
- **Doo-Sabin Subdivision (두-사빈 세분화):**
    - 임의의 다각형 메쉬에 적용되며, 각 면을 새로운 다각형으로 대체합니다.
    - `C¹` 연속성을 보장합니다.

#### b) 세분화 과정 (Catmull-Clark 예시)
1.  **Face Point 생성:** 각 면의 중심에 새로운 정점을 추가합니다.
2.  **Edge Point 생성:** 각 모서리의 중심에 새로운 정점을 추가합니다. 이 점은 원래 모서리의 두 끝점과 인접한 두 면의 Face Point를 이용하여 계산됩니다.
3.  **Vertex Point 이동:** 원래 메쉬의 각 정점의 위치를 인접한 Face Point, Edge Point, 그리고 원래 정점의 가중 평균으로 이동시킵니다.
4.  **새로운 면 생성:** 새로운 정점들을 연결하여 새로운 쿼드 메쉬를 생성합니다.

이 과정을 반복할수록 메쉬는 점점 더 부드러운 곡면으로 수렴합니다. 테셀레이션 쉐이더(Tessellation Shader)를 이용하여 GPU에서 실시간으로 세분화 곡면을 생성할 수 있습니다.

---

## Advanced Shaders I & Review

### Tessellation (테셀레이션) - ACG_C10 보강
- **정의:** GPU가 렌더링 파이프라인 내에서 기하학적 프리미티브(삼각형 등)를 더 잘게 쪼개어 디테일을 높이는 기술입니다.
- **목적 및 장점:**
    - **메모리 효율:** 거친(Coarse/Low-poly) 모델을 메모리에 저장하고 전송한 뒤, GPU에서 실시간으로 디테일을 생성하므로 대역폭을 절약합니다.
    - **LOD (Level of Detail):** 카메라와의 거리에 따라 테셀레이션 레벨을 동적으로 조절하여 성능과 품질의 균형을 맞춥니다.
    - **애니메이션 효율:** 저폴리곤 메쉬로 스키닝 및 애니메이션 연산을 수행하고, 렌더링 직전에 부드럽게 만들어 연산 비용을 줄입니다.
- **Displacement Mapping (변위 매핑):**
    - 테셀레이션된 정점의 위치를 텍스처(Height Map) 정보에 따라 실제로 이동시킵니다.
    - 노멀 매핑은 실루엣이 평평해 보이고 시차 효과가 없는 반면, 변위 매핑은 실제 기하학적 구조를 변경하므로 완벽한 입체감을 제공합니다.

#### 테셀레이션 파이프라인 (Tessellation Pipeline)
OpenGL 4.0 및 DirectX 11부터 도입된 3단계 프로세스입니다. (Vertex Shader와 Geometry Shader 사이에 위치)

1.  **TCS (Tessellation Control Shader):**
    - **입력:** 패치(Patch, 사용자 정의 정점 그룹) 단위.
    - **역할:**
        - **Tessellation Levels 결정:** `gl_TessLevelInner`(내부 분할)와 `gl_TessLevelOuter`(가장자리 분할) 변수를 설정하여 TPG에게 얼마나 잘게 쪼갤지 지시합니다.
        - 레벨이 0 이하이면 해당 패치는 렌더링에서 제외(Culling)됩니다.
    - **제어:** 카메라 거리, 화면 차지 면적, 곡률 등을 기반으로 레벨을 계산합니다.

2.  **TPG (Tessellation Primitive Generator) / Tessellator:**
    - **역할:** 고정 함수(Fixed-function) 하드웨어 단계입니다. TCS에서 설정한 레벨에 따라 추상적인 패치 도메인(사각형, 삼각형, 선)을 실제 정점들로 분할합니다.
    - **출력:** 각 정점의 정규화된 파라메트릭 좌표 `(u, v)` 또는 `(u, v, w)`.

3.  **TES (Tessellation Evaluation Shader):**
    - **입력:** TPG가 생성한 `(u, v)` 좌표와 TCS에서 전달받은 패치 데이터.
    - **역할:** 각 정점의 **최종 월드 위치를 계산**합니다.
        - 베지에 곡면 수식(`B-Spline`, `NURBS` 등)을 적용하거나,
        - 변위 맵(Displacement Map)을 텍스처링하여 정점을 법선 방향으로 이동시킵니다.
    - 버텍스 쉐이더처럼 MVP 행렬 변환을 수행하여 화면에 출력합니다.

### 프로그래밍 가능한 그래픽스 파이프라인 (Programmable Graphics Pipeline)
*   **고정 함수 파이프라인 (Fixed Function Pipeline) 대비:**
    *   **GPU 프론트엔드:** Vtx 인덱스 -> 프리미티브 어셈블리 -> 래스터화 및 보간 -> 래스터 연산 -> 픽셀 업데이트 -> 프레임 버퍼
    *   **프로그래밍 가능한 버텍스 쉐이더 (Programmable Vertex Shader):** 변환된 정점 생성
    *   **프로그래밍 가능한 프래그먼트 쉐이더 (Programmable Fragment Shader):** 변환된 프래그먼트 생성

### 지오메트리 쉐이더 (Geometry Shader)
*   **지오메트리 쉐이더란?**
    *   파이프라인에서 버텍스 쉐이더와 프래그먼트 쉐이더 사이에 위치.
    *   **기능:**
        *   프리미티브 추가/제거 (Add/remove primitives)
        *   정점 추가/제거 (Add/remove vertices)
        *   정점 위치 편집 (Edit vertex position)
    *   **특징:**
        *   한 번에 하나의 프리미티브 처리.
        *   프리미티브 조작, 제거 또는 생성 가능.
        *   원본 프리미티브를 폐기.
        *   결과는 변환된 정점의 새로운 집합.
        *   버텍스 쉐이더를 대체하지 않음.
        *   입력 및 출력 프리미티브 타입이 일치할 필요 없음.
        *   OpenGL 3.2 (2009년 8월 3일)부터 핵심 기능.
*   **파이프라인에서의 위치:**
    *   버텍스 쉐이더 -> 지오메트리 쉐이더 -> 클리핑/뷰포트 변환 -> 래스터화 -> 프래그먼트 쉐이더 -> 프레임버퍼
    *   쉐이더 프로그램은 버텍스, 지오메트리, 프래그먼트 쉐이더의 어떤 조합도 가질 수 있으며, 세 가지 모두 동시에 필요하지는 않음.
*   **설정 (Setup):**
    *   `glCreateShader(GL_GEOMETRY_SHADER)`
    *   `glShaderSource`, `glCompileShader`
    *   `glCreateProgram`, `glAttachShader` (지오메트리, 버텍스, 프래그먼트 쉐이더 모두)
    *   `glUseProgram`
*   **보간 한정자 (Interpolation Qualifier):**
    *   **사용 위치:** 버텍스 쉐이더 출력, 테셀레이션 제어/평가 쉐이더 입력/출력, 지오메트리 쉐이더 입력/출력, 프래그먼트 쉐이더 입력.
    *   **타입:**
        *   `flat`: 보간되지 않음. 프래그먼트 쉐이더에 Provoking Vertex의 값이 전달됨.
        *   `noperspective`: 윈도우 공간에서 선형 보간. 일반적으로 원치 않지만 사용 사례가 있음.
        *   `smooth`: 원근 보정 방식으로 보간 (기본값).
*   **프리미티브 입출력 (Primitive In and Outs):**
    *   **입력:** `GL_POINTS`, `GL_LINES`, `GL_TRIANGLES` 및 `_ADJACENCY` 버전.
    *   **출력:** `GL_POINTS`, `GL_LINE_STRIP`, `GL_TRIANGLE_STRIP`.
    *   입력 프리미티브 타입은 출력 프리미티브 타입과 같을 필요 없음.
    *   출력 프리미티브는 연결되지 않을 수 있음.
*   **내장 입력 변수 (Built-In Input Variables):**
    *   `gl_PrimitiveIDIn`: 현재까지 처리된 프리미티브 수. 프리미티브 어셈블리 단계에서 설정.
    *   `gl_in[]`: 버텍스 쉐이더에서 변환된 `gl_Position`, `gl_PointSize`, `gl_ClipDistance` 등.
*   **지오메트리 언어 (Geometry Language):**
    *   `in gl_PerVertex { ... } gl_in[];`
    *   `in int gl_PrimitiveIDIn;`
    *   `out gl_PerVertex { ... };`
    *   `out int gl_PrimitiveID;`
    *   `out int gl_Layer;`
    *   `out int gl_ViewportIndex;`
*   **간단한 예시:**
    *   **점으로부터 선 생성:** `gl_in[0].gl_Position`을 기준으로 `vec4(-0.1, 0.0, 0.0, 0.0)` 및 `vec4(0.1, 0.0, 0.0, 0.0)` 오프셋을 주어 두 개의 정점을 생성하고 `EmitVertex()`, `EndPrimitive()`로 선 스트립을 그림.
    *   **선으로부터 선 생성 (색상 포함):** 입력 `lines`를 받아 출력 `line_strip`으로 변환. `gl_Position.xy = gl_Position.yx;`와 같이 정점 위치를 조작하고 `colorOut`을 전달.
    *   **베지에 곡선 (간단한 예시):**
        *   **버텍스 쉐이더:** `gl_Position = position; v2gColor = color;`
        *   **지오메트리 쉐이더:** `layout (lines_adjacency) in; layout (line_strip, max_vertices = 128) out;`
            *   `tess` (테셀레이션 레벨)에 따라 `u`를 증가시키며 3차 베지에 곡선 `p = term3 * gl_in[0].gl_Position + ...`을 계산.
            *   `gl_Position = p; EmitVertex();`
        *   **프래그먼트 쉐이더:** `colorOut = g2fColor;`

### 추가 응용 (More Applications)
*   고슴도치 플롯 (Hedgehog Plots)
*   폭발 효과 (Explosion)
*   세분화 곡면 (Subdivision Surface)

### 숙제
*   지오메트리 쉐이더를 사용하여 베지에 곡선 그리기 코드 작성.
    *   입력: 4점, 테셀레이션 레벨.
    *   출력: 베지에 곡선.
    *   제출물: 테셀레이션 레벨 10과 100에 대한 스크린샷, 소스 파일 (C/C++ 및 쉐이더 파일).
    *   주의: 지오메트리 쉐이더를 사용하지 않으면 0점 처리.

### 지오메트리 쉐이더 생성
*   `glCreateShader(GL_GEOMETRY_SHADER);`
*   `glShaderSource`
*   `glCompileShader`
*   `glAttachShader`
*   `glLinkProgram`

### 렌더링 파이프라인과 GLSL
- **Vertex Shader:** 정점 단위 연산. MVP 변환, 정점 속성(UV, Normal) 전달.
- **Fragment Shader:** 픽셀(프래그먼트) 단위 연산. 텍스처 샘플링, 조명 계산, 최종 색상 결정.
- **GLSL 변수 타입:**
    - `attribute` (in): 버텍스 쉐이더 입력 (정점 데이터).
    - `uniform`: CPU에서 전달하는 전역 상수 (모든 쉐이더에서 동일).
    - `varying` (out/in): 버텍스 -> 프래그먼트 쉐이더로 보간되어 전달되는 데이터.

### 고급 쉐이딩 기법
- **Toon (Cel) Shading:**
    - `dot(N, L)` 값을 임계값과 비교하여 명암 단계를 결정.
    - 외곽선: 1) 뒷면을 부풀려 먼저 그리기, 2) 깊이/법선 텍스처를 후처리하여 엣지 검출.
- **Rim Lighting:**
    - `rim = 1.0 - dot(N, V)`. 시선과 법선이 수직에 가까울수록 밝아짐.
- **Normal Mapping:**
    - 저폴리곤 모델에 고폴리곤의 디테일을 입히는 기술.
    - **Tangent Space:** Normal(N), Tangent(T), Bitangent(B)로 구성된 정점별 로컬 좌표계.
    - **과정:** 프래그먼트 쉐이더에서 노멀맵(Tangent Space 기준)을 읽어와 TBN 행렬로 월드/뷰 공간으로 변환 후 조명 계산에 사용.
- **Parallax Mapping:**
    - Height Map을 이용해 UV 좌표를 왜곡시켜 깊이감을 표현.
    - **Parallax Occlusion Mapping (POM):** 레이마칭으로 더 정확한 깊이와 자체 그림자 효과 구현.

---

## Image Processing with Shader (상세 보강)

### 1. 기본 연산 (Basic Operations)
이미지 처리는 픽셀 값을 조작하여 화질을 개선하거나 정보를 추출하는 과정입니다.
- **히스토그램 (Histogram):** 이미지의 밝기 분포를 나타내는 그래프.
    - **평활화 (Equalization):** 픽셀들이 전 밝기 영역에 골고루 분포하도록 비선형 변환을 적용하여 대비(Contrast)를 극대화합니다.
- **점 연산 (Point Operations):** 한 픽셀의 출력 값이 해당 입력 픽셀 값에만 의존합니다.
    - **Thresholding (이진화):** 특정 값 이상은 흰색, 이하는 검은색으로 변환 (배경/물체 분리).
    - **Contrast Stretching:** 좁은 밝기 범위를 전체 영역으로 늘려 선명하게 만듭니다.
- **대수 연산 (Algebraic Operations):** 두 개 이상의 이미지를 픽셀별로 연산합니다.
    - **덧셈 (Addition):** 여러 장의 이미지를 평균내어 랜덤 노이즈를 제거(`Frame Averaging`).
    - **뺄셈 (Subtraction):** 배경 이미지를 빼서 움직이는 물체만 검출(`Motion Detection`)하거나 배경을 제거.
    - **곱셈/나눗셈:** 마스킹(Masking)이나 조명 보정 등에 사용.

### 2. 지역 연산 (Local Operations & Filtering)
출력 픽셀 값이 입력 픽셀과 그 주변 이웃 픽셀들의 값에 의해 결정됩니다. 주로 **컨볼루션(Convolution)** 마스크를 사용합니다.
- **스무딩 (Smoothing/Low Pass):**
    - **Box Filter (Average):** 주변 픽셀의 평균값 사용. 블러링 효과 및 노이즈 감소.
    - **Gaussian Filter:** 중심에 가중치를 둔 평균. 더 자연스러운 블러링.
    - **Median Filter:** 주변 값 중 중간값을 선택. 엣지를 보존하면서 소금-후추 노이즈(Salt & Pepper Noise) 제거에 탁월.
- **엣지 검출 (Edge Detection/High Pass):** 밝기 변화량이 큰 부분을 찾습니다.
    - **Gradient:** 인접 픽셀간의 차이 계산 (`[-1, 1]`).
    - **Sobel Operator:** 수평/수직 방향의 변화를 각각 계산하여 합침. 노이즈에 비교적 강함.
    - **Laplacian:** 2차 미분 기반. 모든 방향의 엣지 검출. (`[[0,1,0],[1,-4,1],[0,1,0]]`)

### 3. 주파수 도메인 (Frequency Domain)
- **푸리에 변환 (Fourier Transform):** 이미지를 공간 도메인에서 주파수 도메인으로 변환합니다. 이미지는 사인파의 합으로 표현됩니다.
- **필터링:** 주파수 영역에서 특정 주파수 대역을 차단하거나 증폭시킨 후 다시 역변환합니다.
    - **Low Pass:** 고주파(상세 정보, 노이즈) 제거 -> 부드러운 이미지 복원.
    - **High Pass:** 저주파(전반적 밝기 분포) 제거 -> 엣지 및 디테일 강조.

### 4. 영상 분석 (Image Analysis)
- **분할 (Segmentation):** 이미지를 의미 있는 영역(객체 vs 배경)으로 나눕니다. (Thresholding, Edge-based 등)
- **경계 추적 (Boundary Tracking):** 객체의 외곽선을 따라가며 좌표를 추출합니다 (Chain Code 등).
- **특징 추출:** 객체를 식별하기 위한 수치적 특징을 계산합니다.
    - **면적(Area), 둘레(Perimeter)**
    - **원형도(Circularity):** `P^2/A` (원일 때 최소값 4π). 모양이 복잡할수록 값이 커짐.
    - **오일러 수(Euler Number):** `객체 수 - 구멍 수`. 위상학적 특징.

---

## ACG_C14-2_FBO: Framebuffer Object (상세 보강)

### 1. 개요
- 기본 프레임버퍼(Default Framebuffer)는 화면(윈도우)에 직접 출력하는 메모리 영역입니다.
- **FBO (Framebuffer Object):** 화면에 보이지 않는(Off-screen) 메모리 버퍼를 생성하여, 렌더링 결과를 **텍스처**나 **렌더버퍼**에 저장할 수 있게 해주는 OpenGL 객체입니다.

### 2. 구성 요소 및 생성 단계
FBO 자체는 데이터를 저장하지 않는 컨테이너이며, 실제 데이터가 저장될 'Attachments'가 필요합니다.

1.  **FBO 생성:** `glGenFramebuffers(1, &fbo)`, `glBindFramebuffer(GL_FRAMEBUFFER, fbo)`.
2.  **Texture Attachment (컬러 버퍼용):**
    - 렌더링 결과를 텍스처 이미지로 저장하여 나중에 쉐이더에서 샘플링할 수 있습니다.
    - `glGenTextures`, `glBindTexture`.
    - `glTexImage2D(..., NULL)`: 데이터를 비워둔 채 메모리만 할당합니다.
    - **부착:** `glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id, 0)`.
3.  **Renderbuffer Attachment (깊이/스텐실 버퍼용):**
    - 텍스처링이 필요 없고(샘플링 안 함) 렌더링 중 깊이/스텐실 테스트용으로만 쓸 때, 텍스처보다 최적화되어 있어 성능상 유리합니다.
    - `glGenRenderbuffers`, `glBindRenderbuffer`.
    - `glRenderbufferStorage(..., GL_DEPTH24_STENCIL8, width, height)`.
    - **부착:** `glFramebufferRenderbuffer(..., GL_DEPTH_STENCIL_ATTACHMENT, ..., rbo_id)`.
4.  **완전성 검사:** `glCheckFramebufferStatus`가 `GL_FRAMEBUFFER_COMPLETE`를 반환하는지 확인해야 합니다.
5.  **렌더링 루프:**
    - `glBindFramebuffer(..., fbo)`: FBO 활성화.
    - `glViewport(...)`: FBO 텍스처 크기에 맞게 뷰포트 설정.
    - 씬 렌더링 (결과가 텍스처에 그려짐).
    - `glBindFramebuffer(..., 0)`: 기본 프레임버퍼로 복귀.
    - `glViewport(...)`: 화면 크기로 복귀.
    - 저장된 텍스처를 사용하여 화면에 사각형을 그리거나(Post-processing), 다른 물체에 입힘(Mirror, CCTV).

### 3. 활용
- **Render-to-Texture:** 거울 반사, 보안카메라 화면, 동적 큐브 맵 등.
- **Post-Processing (후처리):** 씬 전체를 텍스처로 렌더링한 후, 프래그먼트 쉐이더에서 블러, 블룸(Bloom), 톤 매핑, 엣지 강조 등의 효과를 적용하여 최종 출력.
- **Shadow Mapping:** 빛의 시점에서 깊이 값만 렌더링하여 Depth Map(Shadow Map)을 생성.
- **Deferred Shading:** 위치, 법선, 색상 정보를 각각 다른 텍스처(G-Buffer)에 저장한 후, 나중에 조명 연산을 수행.

---

## Computer Animation & Interpolation (ACG_C15, C16 보강)

### 1. 애니메이션 개요
- **정의:** 생명이 없는 물체에 움직임을 부여하여 살아있는 것처럼 보이게 하는 기술.
- **기법:**
    - **Key-frame Animation:** 주요 시점(Keyframe)의 포즈를 잡고, 그 사이를 보간(Interpolation)으로 채움.
    - **Motion Capture:** 실제 사람/동물의 움직임을 센서로 기록하여 적용. 데이터 기반.
    - **Physics-based:** 물리 법칙(힘, 중력, 충돌)을 시뮬레이션하여 움직임 생성.

### 2. 보간 (Interpolation) 기법
주어진 제어점(Control Points)들을 부드럽게 연결하는 곡선을 생성합니다.
- **선형 보간 (Linear):** 두 점을 직선으로 연결. 움직임이 딱딱하고 불연속적(`C0`).
- **다항식 보간 (Polynomial):**
    - **Lagrange:** 모든 점을 통과하는 n차 다항식. 점이 많아지면 진동(Runge's Phenomenon) 발생.
    - **Hermite:** 양 끝점의 위치와 접선(Tangent) 벡터로 정의. 제어가 직관적.
    - **Catmull-Rom Spline:** 제어점만 주어지면 자동으로 부드러운 접선을 계산하여 모든 점을 통과하는 곡선 생성. (이전/다음 점을 이용해 접선 계산).
    - **Bezier Curve:** 시작/끝점만 통과하고 중간 점들은 곡선의 당김(Approximation) 역할. 기하학적 제어가 용이.
    - **B-Spline / NURBS:** 국부 제어(Local Control)가 가능하고 연속성이 보장되는 근사 곡선.

### 3. 속도 제어 (Speed Control)
경로(Path)와 속도(Speed)는 독립적으로 제어되어야 합니다.
- **문제점:** 매개변수 `u`를 등간격으로 증가시켜도, 제어점 간격에 따라 실제 이동 거리(속도)가 불균일할 수 있습니다.
- **Arc Length Reparameterization (호 길이 매개변수화):**
    - `u` 대신 이동 거리 `s`를 함수 인자로 사용 (`p = P(U(s))`).
    - **구현:** `u`에 따른 곡선 길이 `s(u)`를 적분(수치해석 또는 근사)으로 구하여 테이블(Look-up Table)을 만들고, 역함수 `u(s)`를 통해 원하는 거리만큼 이동한 `u`를 찾습니다.
- **Ease-in / Ease-out:**
    - 움직임의 시작과 끝을 부드럽게 가감속 처리.
    - `s = f(t)` 함수를 사용 (Sinusoidal, Cubic, Piecewise Linear 등). `t`가 일정하게 흐를 때 `s`(이동 거리)는 부드럽게 변화.

### 4. 방향 제어 (Orientation Control)
물체가 곡선을 따라 이동할 때 어디를 바라볼지 결정합니다.
- **Frenet Frame:** 곡선의 접선(Tangent), 법선(Normal), 종법선(Binormal)으로 좌표계 구성.
    - 문제점: 변곡점에서 프레임이 급격히 뒤집히거나, 비틀림(Torsion) 문제 발생.
- **Parallel Transport / Reflection:** 이전 프레임의 방향을 최소한으로 회전시켜 다음 프레임의 방향 결정. 급격한 회전 방지.
- **Up Vector 지정:** 카메라 제어 등에서 '위쪽' 방향을 고정하여 비틀림 방지.

### 5. 쿼터니언 (Quaternion) 회전 보간
3차원 회전을 4차원 벡터 `(x, y, z, w)`로 표현합니다.
- **장점:** 짐벌 락(Gimbal Lock) 문제 해결, 오일러 각도보다 보간이 훨씬 부드럽고 자연스러움.
- **구면 선형 보간 (Slerp - Spherical Linear Interpolation):**
    - 두 쿼터니언 `q1`, `q2` 사이를 4차원 구면 위에서 최단 경로(Great Arc)를 따라 등속 회전.
    - `Slerp(q1, q2, t) = (sin((1-t)θ)/sinθ)*q1 + (sin(tθ)/sinθ)*q2`.
- **Squad (Spherical Cubic Interpolation):** 쿼터니언 버전의 베지에 곡선. 연속적인 회전 키프레임을 부드럽게(`C1`, `C2`) 연결. `Bisect`와 `Double` 연산자를 이용해 제어점(Control Quaternion)을 생성하여 보간.

### 6. 계층적 애니메이션 (Hierarchical Animation) - ACG_C19, C20 보강
사람이나 동물처럼 여러 관절로 연결된 복잡한 물체를 애니메이션하기 위해 계층 구조(Tree Structure)를 사용합니다.
- **구조:**
    - **Node (Link/Segment):** 뼈대(Bone)에 해당. 형태를 가짐.
    - **Arc (Joint):** 관절. 부모 노드와 자식 노드 사이의 변환(회전, 이동)을 정의.
- **Forward Kinematics (FK, 순운동학):**
    - 상위(부모) 관절의 변환이 하위(자식) 관절에 순차적으로 전파됨.
    - **제어:** 각 관절의 각도(`θ`)를 직접 입력 -> 말단(End Effector)의 위치가 결정됨.
    - **구현:** 트리 순회(Traversal)를 하며 행렬 스택(Push/Pop)을 이용해 로컬 변환을 누적(`M_global = M_parent * M_local`).

### 7. 역운동학 (Inverse Kinematics, IK)
- **개념:** 말단(End Effector)이 도달해야 할 **목표 위치(Goal Position)**를 주면, 그 위치에 닿기 위한 **각 관절의 각도**를 컴퓨터가 역으로 계산해내는 기법.
- **해석적 방법 (Analytic):**
    - 삼각함수(코사인 제2법칙 등)를 이용해 수식으로 정확한 해를 구함.
    - 계산이 빠르지만, 관절 수가 적고 구조가 간단할 때만 가능.
- **수치적 방법 (Numeric) - 반복적(Iterative) 해법:**
    - **Jacobian Matrix (자코비안 행렬):** 관절 각도의 미세한 변화(`dθ`)가 말단 위치의 미세한 변화(`dX`)에 미치는 영향을 나타내는 행렬. `dX = J * dθ`.
    - **Pseudo-inverse (의사 역행렬):** `J`가 정방행렬이 아닐 때(자유도가 남거나 부족할 때) 역행렬 대신 `J+`를 사용하여 `dθ = J+ * dX`를 구함.
    - **Damped Least Squares:** 팔을 완전히 뻗는 등 특이점(Singularity) 상황에서 값이 발산하여 동작이 튀는 것을 막기 위해 감쇠(Damping) 항을 추가하여 안정화.
    - **CCD (Cyclic Coordinate Descent):** 말단 관절부터 루트 관절까지 차례대로, 현재 관절을 회전시켜 말단을 목표 지점에 가장 가깝게 맞추는 과정을 반복하는 휴리스틱 기법. 구현이 쉽고 빠르며 관절 제한(Joint Limit) 적용이 용이함.

### 8. 모션 캡처 및 리타겟팅 (Motion Capture & Retargeting) - ACG_C21, C22 보강

#### a) 모션 캡처 (Motion Capture)
- 연기자의 몸에 마커를 부착하거나(Optical, Magnetic), 마커 없이(Kinect, Image metrics) 관절의 움직임을 기록하여 디지털 캐릭터에 적용합니다.
- **방식:**
    - **광학식 (Optical):** 정확도가 높으나 마커가 가려지는(Occlusion) 문제 발생 가능. (Passive/Active 마커)
    - **자기식 (Magnetic):** 실시간 처리가 용이하나 금속 물체에 간섭을 받음.
    - **기계식 (Electro-Mechanical):** 외골격 슈트 착용. 정확하지만 움직임이 제한적.

#### b) 모션 리타겟팅 (Motion Retargeting)
- **정의:** 한 캐릭터(Source)의 애니메이션을 신체 비율이나 구조가 다른 캐릭터(Target)에게 적용하는 기술.
- **문제점:**
    - 단순히 관절 각도만 복사하면, 발이 땅에 닿지 않거나(Floating), 땅을 뚫고 들어가거나(Penetration), 자신의 몸을 통과하는(Self-collision) 현상 발생.
    - **Footskate (Foot Sliding):** 발이 고정되어야 할 시점에 미끄러지는 현상.
- **해결 방법 (Spacetime Constraints):**
    - 전체 동작을 시공간(Spacetime) 상의 최적화 문제로 정의합니다.
    - **목적 함수 (Objective Function):** 원본 동작과의 차이를 최소화 (`Minimize |m(t) - m_origin(t)|^2`).
    - **제약 조건 (Constraints):** "특정 시간에 발은 지면(y=0)에 있어야 한다", "손은 특정 지점에 닿아야 한다" 등의 조건을 만족해야 함.
    - **Motion Displacement Map:** 원본 모션에 저주파(부드러운) 변위 맵(`d(t)`)을 더하여 고주파(떨림) 노이즈 없이 동작을 수정합니다.
- **딥러닝 기반 리타겟팅 (Skeleton-Aware Networks):**
    - 서로 다른 위상(Topology)을 가진 골격 간의 리타겟팅을 위해, 공통된 추상 골격(Primal Skeleton)을 정의하고 딥러닝 네트워크(Encoder-Decoder 구조)를 통해 모션을 변환합니다.

---

---

## Animating Virtual Humans: 가상 인간 애니메이션 (ACG_C23, C24 추가)

### 1. 신체 모델링 (Body Modeling)
가상 인간을 만들기 위해서는 기하학적 형태(Geometry)와 움직임을 위한 구조(Structure)가 필요합니다.
- **모델링 방식:** 폴리곤 메쉬(Polygon Mesh)가 가장 일반적이며, 디테일을 위해 세분화 곡면(Subdivision Surfaces)을 사용하기도 합니다.
- **표준 구조 (H-Anim):** 캐릭터의 관절 이름과 계층 구조를 표준화하여 애니메이션 데이터의 호환성을 높입니다 (예: `l_shoulder`, `r_knee`).
- **리깅 (Rigging):** 모델 내부에 뼈대(Skeleton)를 심고, 뼈와 피부(Mesh)를 연결하는 과정입니다.
- **스키닝 (Skinning):**
    - **Linear Blend Skinning (LBS):** 하나의 정점이 여러 뼈의 영향을 받을 때, 각 뼈의 변환 행렬에 가중치(Weight)를 곱해 평균을 냅니다. `v' = Σ (w_i * M_i) * v`.
    - 관절이 굽혀질 때 부피가 줄어드는 '사탕 포장지 현상(Candy-wrapper effect)'이 발생할 수 있습니다.

### 2. 신체 제어 (Body Control)
- **상체 (Reaching):** 손이 특정 위치에 도달하는 동작. 어깨(3자유도), 팔꿈치(1자유도), 손목의 움직임을 조합하여 IK로 해결합니다.
- **하체 (Walking):**
    - 걷기는 주기적인 사이클(Walk Cycle)을 가집니다. (Stance phase vs Swing phase).
    - **Pelvic Transport:** 자연스러운 걷기를 위해 골반은 단순히 이동만 하는 것이 아니라 회전(Rotation), 기울기(Tilt), 측면 이동(Lateral displacement)이 복합적으로 일어납니다.
- **손 (Grasping):** 물체의 형태에 따라 손가락 관절을 제어하여 잡는 동작.

### 3. 얼굴 애니메이션 (Facial Animation)
얼굴은 뼈대가 아닌 근육의 수축/이완으로 피부가 움직이는 복잡한 구조입니다.
- **FACS (Facial Action Coding System):**
    - 심리학자 Ekman이 정의한 얼굴 표정 코딩 시스템.
    - 얼굴의 움직임을 46개의 **Action Units (AU)**로 분해합니다. (예: AU1=눈썹 안쪽 올리기, AU12=입꼬리 당기기).
    - 감정(Emotion)은 여러 AU의 조합으로 표현됩니다 (행복 = AU6 + AU12).
- **구현 방식:**
    - **Blend Shapes (Morph Target):** '웃는 얼굴', '화난 얼굴' 등 미리 만들어진 표정 타겟들을 가중치(0~1)를 주어 섞습니다(Linear Interpolation). 직관적이고 퀄리티가 좋지만, 많은 메모리를 차지합니다.
    - **Muscle-based:** 실제 해부학적 근육 구조를 벡터나 스프링으로 모델링하여 시뮬레이션합니다.
    - **MPEG-4 FBA:** 얼굴 애니메이션을 위한 국제 표준 파라미터(FAP)를 정의하여 저대역폭 전송을 지원합니다.

---

## Dynamics: 동역학 (ACG_C25 보강)

### 1. 기본 물리 법칙
- **뉴턴의 제2법칙:** `F = ma` (힘 = 질량 × 가속도). 물체의 움직임을 결정하는 기본 식입니다.
- **상태 변수:** 위치(`x`), 속도(`v`), 가속도(`a`), 운동량(`P = mv`).
- **수치 적분 (Numerical Integration):** 미분방정식을 시간 단위(`dt`)로 근사하여 푸는 과정.
    - `v_new = v_old + a * dt`
    - `x_new = x_old + v_new * dt`
- **적분 방법:**
    - **Explicit Euler:** 가장 간단하지만 오차가 누적되어 불안정합니다(에너지 발산).
    - **Runge-Kutta 4 (RK4):** 한 스텝 내에서 4번의 기울기를 계산하여 가중 평균을 내므로 매우 정확하고 안정적입니다.

### 2. 힘의 종류 (Forces)
- **중력 (Gravity):** `F = mg`. 지구 중심 방향으로 당기는 힘.
- **마찰력 (Friction):** 물체의 운동을 방해하는 힘.
    - 정지 마찰력 (`F_s <= μ_s * N`): 물체가 움직이지 않을 때 버티는 힘.
    - 운동 마찰력 (`F_k = μ_k * N`): 움직이는 물체에 작용하는 힘.
- **점성/항력 (Viscosity/Drag):** 유체(공기, 물) 속에서 움직일 때 받는 저항. 속도에 비례하고 방향은 반대입니다. `F_drag = -k * v`.
- **스프링-댐퍼 (Spring-Damper):** 탄성체 모델링의 핵심.
    - **Hooke's Law (탄성력):** `F_s = k_s * (L_current - L_rest)`. 원래 길이로 돌아가려는 힘.
    - **Damping (감쇠력):** `F_d = -k_d * v_relative`. 진동을 멈추게 하는 힘.
    - **총 힘:** `F = F_s + F_d`.

### 3. 충돌 (Collision)
- **충돌 감지 (Detection):** 기하학적 교차 검사.
- **충돌 반응 (Response):** 충돌 후 속도와 위치 보정. 충격량(Impulse)을 가하여 순간적으로 속도를 변화시킵니다.
- **운동량 보존 (Conservation of Momentum):** 외부 힘이 없을 때, 충돌 전후의 총 운동량은 같습니다. `Σmv_before = Σmv_after`.
- **충돌 종류:**
    - **탄성 충돌 (Elastic):** 운동 에너지가 보존됨 (튕겨 나감).
    - **비탄성 충돌 (Inelastic):** 운동 에너지가 손실됨 (찌그러짐, 열 발생).
    - **반발 계수 (Coefficient of Restitution, e):** `v_rel_after = -e * v_rel_before`. (1: 완전 탄성, 0: 완전 비탄성).

### 4. 강체 동역학 (Rigid Body Dynamics)
- **정의:** 힘을 받아도 형태가 변하지 않는 물체.
- **회전 운동:** 질량(`m`) 대신 **관성 텐서(Inertia Tensor, I)**, 힘(`F`) 대신 **토크(Torque, τ)**, 속도(`v`) 대신 **각속도(ω)**를 사용합니다.
    - `τ = I * α` (토크 = 관성 모멘트 × 각가속도).
    - `L = I * ω` (각운동량 = 관성 모멘트 × 각속도).
- **관성 텐서 (I):** 회전축에 따른 회전 저항성을 나타내는 3x3 행렬. 물체의 형상과 질량 분포에 따라 결정됩니다.

### 5. 입자 시스템 (Particle Systems)
- 연기, 불, 비, 폭발 등 형태가 불분명한 현상을 수많은 점(Particle)으로 표현합니다.
- **생명 주기 (Lifecycle):**
    1.  **생성 (Birth):** 발생원(Emitter)에서 초기 위치, 속도, 수명 등을 랜덤하게 할당하여 생성.
    2.  **갱신 (Update):** 매 프레임마다 힘(중력, 바람 등)을 적용하여 속도/위치 갱신, 수명 감소.
    3.  **렌더링 (Render):** 점, 빌보드(Billboard), 궤적 등으로 화면에 그림.
    4.  **소멸 (Death):** 수명이 다하거나 화면을 벗어나면 제거.

---

## Fluid Animation: 유체 애니메이션 (ACG_C26 추가)

### 1. 유체의 특성
- **비정형성:** 고정된 형태가 없고 부피(Volume)를 가집니다.
- **비압축성 (Incompressible):** 물과 같이 압력을 가해도 부피가 거의 줄어들지 않는 성질. `∇·v = 0` (속도장의 발산이 0).
- **점성 (Viscosity):** 끈적임의 정도. (꿀 > 물).

### 2. 시뮬레이션 방식
- **Lagrangian (라그랑지안, 입자 기반):**
    - 유체를 수많은 입자(Particle)로 봅니다. 각 입자의 이동을 추적합니다.
    - **SPH (Smoothed Particle Hydrodynamics):** 커널 함수를 이용해 주변 입자들의 영향을 부드럽게 합산하여 밀도와 압력을 계산합니다. 물방울 표현에 유리.
- **Eulerian (오일러리안, 격자 기반):**
    - 공간을 고정된 격자(Grid)로 나눕니다. 유체가 격자를 통과할 때 각 셀(Cell)의 속도, 밀도, 압력 변화를 계산합니다.
    - **Stable Fluids:** 격자 기반의 안정적인 시뮬레이션 기법. 전체적인 흐름 표현에 유리.

### 3. Navier-Stokes Equation (나비에-스토크스 방정식)
유체의 운동을 기술하는 가장 중요한 방정식입니다. `F = ma`의 유체 버전입니다.
- **구성 요소:**
    - **이류 (Advection):** 유체의 흐름에 따라 속도나 물질이 이동하는 현상. `-(v·∇)v`.
    - **압력 (Pressure):** 높은 곳에서 낮은 곳으로 미는 힘. `-∇p`.
    - **점성 (Viscosity):** 흐름을 저항하고 퍼뜨리는 힘. `μ∇²v`.
    - **외력 (External Force):** 중력 등. `f`.
- **식:** `∂v/∂t = -(v·∇)v - ∇p + μ∇²v + f`

### 4. 기타 동역학 응용
- **Cloth Simulation:** 질량-스프링 모델(Mass-Spring)을 격자 형태로 연결하여 천을 표현. 구조적(Structural), 전단(Shear), 굽힘(Bend) 스프링을 조합하여 질감 조절.
- **Hair/Fur:** 머리카락 가닥을 체인 형태의 입자-스프링이나 강체 연쇄로 모델링.

---

## Ray Tracing

### Ray Tracing Acceleration (가속화 기법) - ACG_C28_29 보강
레이 트레이싱의 가장 큰 병목은 수백만 개의 광선과 장면 내 모든 물체 간의 교차 검사(Intersection Test)입니다. 이를 `O(N)`에서 `O(log N)` 수준으로 줄이기 위해 가속 구조(Acceleration Structure)가 필수적입니다.

#### 1. Bounding Volumes (경계 볼륨)
복잡한 물체를 단순한 기하학적 형태(구, 상자)로 감싸서, 광선이 이 볼륨과 충돌하지 않으면 내부의 물체와도 충돌하지 않음을 이용하여 연산을 건너뜁니다.
- **Bounding Sphere (경계 구):** 중심과 반지름으로 정의. 검사가 가장 빠르지만, 길쭉한 물체는 빈 공간이 많아 효율이 떨어집니다.
- **AABB (Axis-Aligned Bounding Box):** 축에 정렬된 상자(`x_min`, `x_max`, ...). 생성과 교차 검사가 빠르지만, 물체가 회전하면 상자를 다시 계산해야 하며 핏(Fit)이 느슨해질 수 있습니다.
- **OBB (Oriented Bounding Box):** 물체와 함께 회전하는 상자. 물체에 딱 맞게(Tight) 감쌀 수 있어 교차 검사 횟수를 줄이지만, OBB 자체의 교차 검사 비용이 비쌉니다. (PCA 등을 이용해 생성)

#### 2. Spatial Partitioning (공간 분할)
공간 자체를 나누어 광선이 지나가는 영역(Voxel/Cell)에 있는 물체만 검사합니다.
- **Uniform Grid (균일 격자):** 공간을 일정한 크기의 격자로 나눕니다. 구현이 쉽고 3D DDA 알고리즘으로 빠르게 탐색할 수 있습니다. "Teapot in a stadium" 문제(빈 공간이 많은 씬)에 취약합니다.
- **Octree (옥트리):** 공간을 재귀적으로 8개의 작은 정육면체로 분할합니다. 빈 공간은 큰 노드로 두고 복잡한 공간만 깊게 분할하여 메모리를 절약합니다.
- **K-d Tree:** 축에 수직인 평면으로 공간을 이진 분할합니다. 레이 트레이싱에서 매우 효율적이며 널리 사용됩니다. SAH(Surface Area Heuristic)를 사용하여 최적의 분할 위치를 결정합니다.

#### 3. Bounding Volume Hierarchies (BVH)
물체들을 트리 구조로 계층화합니다. 공간 분할과 달리 물체 기반 분할이므로 자식 노드끼리 영역이 겹칠 수 있습니다.
- **장점:** 물체가 이동해도 트리 구조 자체를 유지하거나 일부만 업데이트(Refitting)하기 용이하여 **동적 씬(Dynamic Scene)**에 적합합니다. 현대의 실시간 레이 트레이싱(RTX 등)은 주로 BVH를 사용합니다.

### Monte Carlo Integration (몬테 카를로 적분)
렌더링 방정식(Rendering Equation)은 해석적으로 풀기 어렵기 때문에, 무작위 샘플링을 통해 근사값을 구하는 몬테 카를로 방법을 사용합니다. `E[f] ≈ (1/N) * Σ f(Xi)`
- **Importance Sampling (중요도 샘플링):** 빛이 많이 들어올 것으로 예상되는 방향(예: 광원 방향, BRDF의 스펙큘러 로브 방향)으로 더 많은 샘플을 할당하여, 적은 샘플 수로도 노이즈를 효과적으로 줄이는 기법입니다.

### 기본 원리
- 카메라에서 픽셀로 광선을 쏴서 교차하는 첫 번째 물체의 색상을 계산하는 방식.
- **장점:** 물리적으로 정확한 반사, 굴절, 그림자.
- **단점:** 래스터라이제이션보다 계산 비용이 매우 높음.

### Whitted-Style Ray Tracing (재귀적 방식)
1.  **Primary Ray:** 카메라 -> 픽셀. 가장 가까운 교차점 `p` 찾기.
2.  **Shading:** `p`에서 로컬 조명 계산.
3.  **Shadow Ray:** `p` -> 광원. 가려지면 그림자.
4.  **Reflection Ray:** `p`에서 반사 방향으로 재귀 호출.
5.  **Refraction Ray:** `p`에서 굴절 방향으로 재귀 호출.

### 가속 구조 (Acceleration Structure)
- 모든 물체와의 교차 검사를 피하기 위함.
- **Bounding Volume Hierarchy (BVH):** 씬을 계층적 경계 상자로 나누어, 광선이 통과하지 않는 영역을 통째로 건너뛰는 방식.

### Path Tracing
- 렌더링 방정식을 몬테카를로 방식으로 푸는 레이 트레이싱의 일반화된 형태.
- 교차점에서 여러 방향으로 광선을 무작위 샘플링하여 빛을 추적.