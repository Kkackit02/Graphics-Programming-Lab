# 작업 요약: Vulkan 레이 트레이싱 프로젝트 수정 내역 (보고서용)

이 문서는 Vulkan Ray Tracing 프로젝트에서 카메라 제어, 조명 및 그림자 기능 구현 과정과 발생했던 문제점 및 해결책을 정리합니다.

## 1. 카메라 이동 및 시점 제어 기능 구현

### 목표
키보드 입력을 통해 장면 내에서 카메라를 자유롭게 이동하고 회전시키는 기능을 구현합니다.

### 수정한 파일
*   `Vulkan_ex01_vn.cpp`

### 상세 설명
기존 코드에는 키보드 입력(`keyCallback`)과 시간(`deltaTime`) 계산 로직이 있었지만, 카메라의 위치나 방향을 업데이트하는 코드는 없었습니다. `processInput` 함수를 새로 추가하고, `mainLoop`에서 매 프레임 이 함수를 호출하여 카메라 제어를 가능하게 했습니다.

#### 변경 내용
1.  **`processInput` 함수 추가**:
    *   `deltaTime`을 이용하여 프레임률에 독립적인 카메라 이동(`camera.position`) 및 회전(`camera.yaw`, `camera.pitch`)을 구현했습니다.
    *   W, A, S, D, Q, E 키로 카메라를 전후좌우 및 상하로 이동시키고, 방향키로 카메라의 시점을 회전시킵니다.
    *   회전 값 (`pitch`)을 ±89도로 제한하여 카메라가 뒤집히는 것을 방지합니다.
    *   변경된 `yaw`와 `pitch` 값으로부터 새로운 `camera.front` 벡터를 계산하여 카메라의 시야 방향을 업데이트합니다.
2.  **`mainLoop` 수정**: `glfwPollEvents()` 호출 직후 `processInput(deltaTime);`을 호출하여 매 프레임 키 입력을 처리하도록 했습니다.

---

## 2. 레이 트레이싱 셰이더 핵심 기능 구현 (조명 및 그림자)

### 목표
레이 트레이싱을 통해 물체에 대한 Blinn-Phong 조명 모델을 적용하고, 사실적인 그림자를 구현합니다. 이 과정에서 발생한 여러 셰이더 및 Vulkan 호환성 문제들을 해결했습니다.

### 수정한 파일
*   `shaders/raygen.rgen`
*   `shaders/closesthit.rchit`
*   `shaders/miss.rmiss`
*   `shaders/shadow.rmiss`
*   `Vulkan_ex01_vn.cpp`

### 상세 설명

#### 2.1. 초기 조명 및 그림자 계산 로직 구현
*   **`closesthit.rchit`**:
    *   물체의 월드 좌표(`worldPos`)와 월드 법선(`worldNormal`)을 계산합니다.
    *   `getShadingNormal()` 함수에서 모델의 법선 데이터를 보간하여 사용하고, 이를 `gl_ObjectToWorldEXT` 행렬로 월드 공간으로 변환합니다.
    *   앰비언트(Ambient), 디퓨즈(Diffuse), 스페큘러(Specular) 광원 요소를 포함하는 Blinn-Phong 조명 모델을 구현했습니다.
    *   각 광원에 대해 그림자 광선(`traceRayEXT`)을 발사하여 그림자 여부를 판단하고, 그림자가 없는 경우에만 디퓨즈 및 스페큘러 조명을 적용합니다.
*   **`raygen.rgen`**: 각 픽셀에 대해 카메라 광선을 발사하는 `traceRayEXT` 호출을 구현하고, Closest Hit 셰이더로부터 받은 색상을 출력 이미지에 저장합니다.

#### 2.2. '이상한 큐브' 문제 및 그림자 렌더링 오류 해결
초기 구현 후 큐브 모델에서 그림자 아티팩트(`그림자가 내부에 생기는 느낌`)와 부자연스러운 셰이딩 문제가 발생했습니다. 이는 주로 그림자 광선 처리 방식과 셰이더 간의 데이터 인터페이스 불일치에서 기인했습니다.

*   **해결책의 핵심 1: 효율적인 그림자 광선 처리 (`gl_RayFlagsSkipClosestHitShaderEXT`)**
    *   **문제점**: 그림자 광선이 물체에 히트했을 때, `closesthit.rchit` 셰이더가 불필요하게 호출되어 처리 오버헤드와 아티팩트를 유발했습니다.
    *   **해결**: 그림자 광선을 쏠 때 `gl_RayFlagsSkipClosestHitShaderEXT` 플래그를 `traceRayEXT` 호출에 추가했습니다. 이 플래그는 그림자 광선이 물체에 히트하더라도 해당 물체의 Closest Hit 셰이더를 호출하지 않고, 그림자 페이로드(`isShadowed`)만 `true`로 설정 후 즉시 종료하도록 하여 그림자 테스트를 매우 효율적이고 견고하게 만듭니다.

*   **해결책의 핵심 2: 페이로드(Payload) 인터페이스 통일**
    *   **문제점**: 셰이더 간에 데이터를 주고받는 페이로드 변수(`hitValue`, `isShadowed`)의 선언 방식이 일치하지 않거나, Vulkan 규칙에 맞지 않아 셰이더 파이프라인 생성 오류가 발생했습니다.
    *   **해결**: 모든 셰이더 파일(`raygen.rgen`, `closesthit.rchit`, `miss.rmiss`, `shadow.rmiss`)에서 다음과 같이 페이로드 인터페이스를 통일했습니다.
        *   `layout(location = 0) rayPayloadEXT vec3 hitValue;` (색상 데이터)
        *   `layout(location = 1) rayPayloadEXT bool isShadowed;` (그림자 여부 데이터)
    *   각 미스 셰이더는 자신의 역할에 맞는 페이로드만 `rayPayloadInEXT`로 받도록 수정했습니다.

*   **그림자 아크네(Shadow Acne) 방지**:
    *   그림자 광선을 쏠 때, 광선 시작점(`worldPos`)을 물체 표면의 법선 방향으로 미세하게(`0.001`만큼) 띄워주는 `offsetOrigin` 기법을 적용했습니다. 이는 부동소수점 오차로 인한 광선이 자기 자신과 충돌하는 문제를 방지합니다.



---

## 3. 스페이스바 광원 토글 기능 구현 (현재 코드 동작 방식)

### 목표
스페이스바를 누르면 장면의 주요 광원을 껐다 켰다 하는 기능을 구현합니다.

### 수정한 파일
*   `Vulkan_ex01_vn.cpp`

### 상세 설명
현재 코드의 구현 방식은 `keyCallback` 함수 내에서 스페이스바 (`GLFW_KEY_SPACE`)가 *해제될 때* 광원의 `enabled` 상태를 토글합니다. 이는 키보드 이벤트에 의해 한 번만 상태가 변경됩니다.

#### 변경 내용
1.  **`keyCallback` 함수 내 로직**:
    ```cpp
    // Vulkan_ex01_vn.cpp 의 keyCallback 함수 내
    // ...
    } else if (action == GLFW_RELEASE) {
        app->keys[key] = false;
        if (key == GLFW_KEY_SPACE) {
            app->lights[0].enabled = 1 - app->lights[0].enabled;
        }
    }
    ```
    *   이 코드는 스페이스바 (`GLFW_KEY_SPACE`)가 *해제되는 시점*에 `lights[0].enabled`의 값을 0과 1 사이로 전환합니다.
    *   **동작 특성**: 스페이스바를 누르고 있다가 떼는 순간 광원의 상태가 한 번 변경됩니다. 이는 짧게 누르거나 길게 누르거나 관계없이 키가 '떼어지는' 이벤트에 반응합니다.
