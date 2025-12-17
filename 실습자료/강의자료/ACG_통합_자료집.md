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

## Image Processing with Shader

### 이미지 처리 (Image Processing)
*   **GPGPU (General-Purpose computation on Graphics Processing Units):** GPU를 이용한 일반 목적 컴퓨팅.
*   **입력:** 이미지
*   **출력:** 처리된 이미지
*   **커널 (Kernel):** 각 픽셀에서 실행되는 연산.
*   **이미지 처리 예시:** 이미지 반전 (Image Negative), 엣지 검출 (Edge Detection), 툰 렌더링 (Toon Rendering).
*   **이미지 처리 질문:**
    *   GPU가 이미지 처리에 적합한가? (데이터 병렬성)
    *   버스 트래픽은?
    *   어떤 타입의 쉐이더가 이미지 처리 커널을 구현해야 하는가? (프래그먼트 쉐이더)

### 이미지 처리: GPU 설정 (GPU Setup)
*   **입력:** 텍스처, 뷰포트 정렬 쿼드 (Viewport-aligned quad, 전체 화면 쿼드)
*   **출력:** 프레임버퍼
*   **커널:** 프래그먼트 쉐이더
*   **GPU 설정 과정:**
    1.  뷰포트 정렬 쿼드 렌더링.
    2.  각 화면 픽셀에 대해 프래그먼트가 호출됨.
    3.  각 프래그먼트 쉐이더는 텍셀로 저장된 이미지의 어떤 부분에도 접근 가능 (`gather`).
    4.  각 프래그먼트 쉐이더는 커널을 실행하고 프레임버퍼에 색상을 기록.
*   **텍스처 좌표 저장:**
    *   **버텍스 쉐이더:** `in vec3 Position; in vec2 Texcoords; out vec2 fs_Texcoords;`
    *   **프래그먼트 쉐이더:** `uniform sampler2D u_Image; in vec2 fs_Texcoords; out vec4 out_Color;`
    *   `gl_Position = vec4(Position, 1.0); fs_Texcoords = Texcoords;`
    *   `out_Color = texture(u_Image, fs_Texcoords);`
*   **프래그먼트 쉐이더에서 텍스처 좌표 계산:**
    *   `uniform vec2 u_inverseViewportDimensions;`
    *   `vec2 txCoord = u_inverseViewportDimensions * gl_FragCoord.xy;`
    *   `out_Color = texture(u_Image, txCoord);`
    *   `u_inverseViewportDimensions`는 뷰포트 크기의 역수.
*   **인접 텍셀 접근 방법:**
    *   `textureOffset()` 사용: 오프셋은 상수여야 함 (`ivec2(x, y)`와 같은 변수는 허용되지 않음).
    *   `textureSize()`와 `delta`를 이용한 수동 계산:
        *   `vec2 delta = 1.0 / textureSize(u_Image);`
        *   `texture(u_Image, txCoord + (delta * vec2(-1.0, 0.0)));`

### Convolution (합성곱)
- 커널(필터) 행렬을 이미지 전체에 이동시키며 적용하는 연산.
- **예시 커널:**
    - **Box Blur:** `[[1,1,1], [1,1,1], [1,1,1]] / 9`
    - **Sharpen:** `[[0,-1,0], [-1,5,-1], [0,-1,0]]`
    - **Sobel (Edge Detection):** 수평/수직 엣지를 검출하는 별도의 커널 사용.
- **쉐이더 구현:** 현재 픽셀 주변의 텍스처를 여러 번 샘플링(`texture()`)하여 커널 값과 곱하고 더함.

### Frequency Domain Filtering
- **Fourier Transform:** 이미지를 공간 영역 -> 주파수 영역으로 변환.
- **Low-pass Filter:** 고주파(엣지, 노이즈)를 제거 -> 블러, 노이즈 감소.
- **High-pass Filter:** 저주파(부드러운 영역)를 제거 -> 엣지 강조.

### 이미지 처리: 커널 예시 (Kernel Examples)
*   **이미지 반전 (Image Negative):** `out_Color = vec4(1.0) - texture(u_Image, fs_Texcoords);`
*   **가우시안 블러 (Gaussian Blur):**
    *   3x3 가우시안 블러 필터: `1/16 * [[1,2,1], [2,4,2], [1,2,1]]`
    *   필터 요소의 합은 1.
    *   엣지 검출, 샤프닝, 엠보싱 등 다른 필터도 사용 가능.
    *   프래그먼트 쉐이더 구현 방법, 메모리 일관성 (3x3, 5x5 등)

### 이미지 처리: Read backs (`glReadPixels`)
*   **`glReadPixels`:** 프레임버퍼의 컬러 버퍼를 읽어와 CPU 메모리에 할당된 버퍼에 저장.
*   **문제점:** GPU에서 CPU로 데이터를 다시 읽어오는 것은 성능 저하를 야기하는 주요 문제.
*   **사용 사례:** 포토샵 유형 애플리케이션.
*   **`glReadPixels`가 필요 없는 경우:** 게임의 후처리, 실시간 비디오 조작, 증강 현실.

---

## ACG_C14-2_FBO

### Framebuffer Object (FBO)
- **정의:** 화면(Default Framebuffer)이 아닌, 사용자가 생성하는 오프스크린 버퍼. 렌더링 결과를 텍스처에 쓸 수 있게 함.
- **구성:**
    - **Color Attachment:** 렌더링된 색상 이미지를 저장할 텍스처.
    - **Depth/Stencil Attachment:** 깊이/스텐실 값을 저장할 텍스처 또는 렌더버퍼.
- **활용:**
    - **Render-to-Texture:** 거울, CCTV 화면, 동적 텍스처 생성.
    - **Post-Processing:** 씬을 텍스처에 렌더링 후, 해당 텍스처에 블러, DoF, 색상 보정 등 전체 화면 효과 적용.
    - **Deferred Shading:** G-Buffer(위치, 법선, Albedo 등)를 여러 텍스처에 렌더링 후, 한 번의 조명 패스에서 최종 색상 계산.

---

## Interpolation

- **Linear (Lerp):** `P(t) = (1-t)P0 + tP1`. C0 연속성.
- **Cubic (Hermite, Bezier):** C1 연속성. 부드러운 움직임.
- **Catmull-Rom Spline:**
    - 제어점을 모두 통과하는 보간 곡선. 키프레임 애니메이션에 적합.
    - `P1`과 `P2` 사이를 보간하기 위해 `P0`와 `P3`를 접선 계산에 사용.
- **Quaternion Interpolation (회전 보간):**
    - **Slerp (Spherical Linear Interpolation):** 짐벌락 없이 두 쿼터니언 사이를 최단 경로로 부드럽게 보간.
      `q_slerp(q0, q1, t) = q0 * (q0^-1 * q1)^t`

---

## Animation

### 보간 기반 애니메이션 (Keyframing)
- 애니메이터가 키프레임의 포즈를 만들면, 그 사이를 컴퓨터가 보간.
- **Animation Curve:** 시간 대 속성 값의 그래프.
- **Graph Editor:** 이 커브를 직접 편집하여 가속/감속 등 미세한 제어.

### 계층적 애니메이션
- **Forward Kinematics (FK):** 부모 관절의 변환이 자식에게 상속. 직관적이지만, 말단 제어가 어려움.
  `World_Child = World_Parent * Local_Child`
- **Inverse Kinematics (IK):** 말단 이펙터의 목표 위치를 주면, 관절 체인의 회전값을 역으로 계산.
  - **알고리즘:** CCD, FABRIK. 반복적(iterative) 해법이 일반적.

### 인간형 애니메이션
- **Skinning (Vertex Blending):** 뼈대의 움직임에 따라 메쉬를 변형.
    - **Linear Blend Skinning (LBS):** 각 정점 위치를, 영향을 주는 뼈들의 변환 행렬에 가중치를 곱해 선형 혼합. 관절이 접힐 때 부피가 줄어드는 문제 발생.
    - **Dual Quaternion Skinning (DQS):** 듀얼 쿼터니언으로 회전과 이동을 함께 보간. LBS의 부피 손실 문제를 개선.

### 모션 리타겟팅
- 한 캐릭터의 모션을 다른 비율의 캐릭터에 적용.
- **과정:** 뼈대 매핑 -> 로컬 회전값 복사 -> IK와 위치 조정을 통해 발 미끄러짐 등 아티팩트 수정.

---

## Dynamics

### 입자 시스템 (Particle System)
- **속성:** 위치, 속도, 가속도, 수명, 질량 등.
- **업데이트 루프 (수치 적분):**
  `force = ...`
  `acceleration = force / mass`
  `velocity += acceleration * dt`
  `position += velocity * dt`
- **적분 방법:**
    - **Explicit Euler:** 간단하지만 불안정.
    - **Verlet, Runge-Kutta 4 (RK4):** 더 정확하고 안정적.

### 강체 동역학 (Rigid Body Dynamics)
- **상태:** 위치, 회전(쿼터니언), 선형/각 운동량.
- **충돌 처리:**
    1. **Collision Detection:** AABB, Sphere, OBB, GJK 알고리즘 등으로 충돌 여부 판별.
    2. **Collision Response:** 충돌 지점, 법선, 깊이를 계산. 충격량(Impulse) 기반으로 운동량을 갱신하여 겹침을 해결하고 반발력을 적용.

---

## Ray Tracing

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
- **결과:** 간접광, 부드러운 그림자, 색상 번짐 등 매우 사실적인 전역 조명(Global Illumination) 효과를 얻을 수 있지만, 노이즈가 많아 많은 수의 샘플이 필요.

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