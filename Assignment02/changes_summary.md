# 작업 요약: Vulkan 레이 트레이싱 프로젝트 수정 내역 (상세)

이 문서는 프로젝트가 정상적으로 작동하기까지 진행된 일련의 수정 작업을 상세히 기록합니다.

## 1. 카메라 이동 기능 구현

### 목표
키보드 입력을 통해 장면 안에서 자유롭게 카메라를 이동하고 회전시키는 기능을 구현합니다.

### 수정한 파일
*   `Vulkan_ex01_vn.cpp`

### 상세 설명
기존 코드에는 키보드 입력을 받는 `keyCallback` 함수는 있었지만, 실제 눌린 키에 따라 카메라의 위치와 방향을 계산하는 로직이 없었습니다. 이를 위해 `processInput` 함수를 새로 추가하고, 매 프레임 이 함수를 호출하도록 `mainLoop`를 수정했습니다.

#### 변경 내용
1.  **`processInput` 함수 추가**:
    *   `deltaTime` (프레임 간 시간 간격)을 인자로 받아, 프레임률에 관계없이 일정한 속도로 카메라가 움직이도록 보정합니다.
    *   W, A, S, D, Q, E 키에 따라 카메라의 `position` 값을 `front` (앞뒤), `cross(front, up)` (좌우), `up` (상하) 벡터 기준으로 변경합니다.
    *   방향키(UP, DOWN, LEFT, RIGHT)에 따라 카메라의 `yaw`와 `pitch` 값을 변경하여 시야를 회전시킵니다.
    *   `pitch` 값이 ±89도를 넘어가지 않도록 제한하여, 카메라가 뒤집히는 현상을 방지합니다.
    *   변경된 `yaw`와 `pitch` 값으로부터 새로운 `front` 방향 벡터를 삼각함수를 이용해 다시 계산합니다.

    ```cpp
    // Vulkan_ex01_vn.cpp에 추가된 함수
    void processInput(float deltaTime) {
        float cameraSpeed = camera.speed * deltaTime;
        if (keys[GLFW_KEY_W])
            camera.position += cameraSpeed * camera.front;
        // ... (S, A, D, Q, E 키에 대한 로직) ...

        float cameraAngularSpeed = camera.angularSpeed * deltaTime;
        if (keys[GLFW_KEY_LEFT])
            camera.yaw -= cameraAngularSpeed;
        // ... (방향키 로직) ...

        // 새로운 front 벡터 계산
        glm::vec3 front;
        front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        front.y = sin(glm::radians(camera.pitch));
        front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.front = glm::normalize(front);
    }
    ```

2.  **`mainLoop` 수정**:
    *   `glfwPollEvents()` 호출 다음에 `processInput(deltaTime);` 호출을 추가하여 매 프레임마다 키 입력을 처리하도록 했습니다.

    ```cpp
    // Vulkan_ex01_vn.cpp의 수정된 mainLoop
    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            // ... deltaTime 계산 ...

            glfwPollEvents();
            processInput(deltaTime); // 이 부분 추가

            drawFrame();
        }
        vkDeviceWaitIdle(device);
    }
    ```

---

## 2. 레이 트레이싱 셰이더 오류 수정 및 기능 완성

### 목표
초기 버전 셰이더의 여러 오류를 수정하고, 빛과 상호작용하는 사실적인 그림자와 조명이 렌더링되도록 기능을 완성합니다. 이 과정은 여러 단계의 오류 수정 작업을 포함했습니다.

### 수정한 파일
*   `shaders/raygen.rgen`
*   `shaders/closesthit.rchit`
*   `shaders/miss.rmiss`
*   `shaders/shadow.rmiss`
*   `Vulkan_ex01_vn.cpp`

### 상세 설명

#### 2.1. 셰이더 Payload 구조 통일 (핵심 수정)
*   **문제점**: "Entry-point has more than one variable with the IncomingRayPayloadKHR" 오류 발생. Vulkan 레이 트레이싱 사양에 따르면, 하나의 셰이더는 `rayPayloadInEXT` 키워드를 단 한 번만 사용하여 하나의 데이터 패킷만 받을 수 있습니다. 하지만 이전 코드에서는 `hitValue`와 `isShadowed`를 별개의 변수로 받으려고 시도하여 이 규칙을 위반했습니다.
*   **해결책**: 색상, 그림자 확인 플래그 등 여러 데이터를 하나의 `struct` (구조체)로 묶어 '단일 데이터 패킷'으로 만들어 전달하도록 수정했습니다.
*   **변경 내용**:
    *   모든 셰이더 파일(`raygen`, `miss`, `shadow.rmiss`, `closesthit`)에 아래와 같은 `RayPayload` 구조체 정의를 추가했습니다.

        ```glsl
        struct RayPayload {
            vec3 hitValue;
            bool isOccluded;    // 그림자에 가려졌는지 여부
            bool isShadowRay;   // 이 광선이 그림자 확인용인지 여부
        };
        ```
    *   모든 셰이더에서 `layout(location = ...)` 선언을 `layout(location = 0) rayPayloadEXT RayPayload prd;` 또는 `rayPayloadInEXT RayPayload prd;`로 통일했습니다.

#### 2.2. 그림자 구현 로직 수정
*   **문제점**: 그림자 광선이 물체에 부딪혔을 때, "그림자가 졌다"고 판단만 하고 멈춰야 하는데, 불필요하게 또다시 복잡한 조명 계산 로직(자기 자신)을 호출하여 그림자가 제대로 생성되지 않았습니다.
*   **해결책**: `closesthit.rchit` 셰이더의 시작 부분에 '꼬리표'를 확인하는 로직을 추가했습니다.
*   **변경 내용**:
    *   `closesthit.rchit`의 `main` 함수 맨 처음에 아래 코드를 추가하여, 그림자 광선(`isShadowRay == true`)일 경우 `isOccluded` 플래그만 `true`로 설정하고 즉시 처리를 종료하도록 했습니다.

        ```glsl
        // closesthit.rchit 수정 내용
        void main() {
            if (prd.isShadowRay) {
                prd.isOccluded = true;
                return;
            }
            // ... (이하 카메라 광선에 대한 조명 계산 로직) ...
        }
        ```
    *   그림자 광선을 추적하기 직전에 `prd.isShadowRay = true;`로 꼬리표를 설정하고, 추적이 끝나면 `if (!prd.isOccluded)`로 그림자 여부를 확인하여 조명을 계산하도록 로직을 수정했습니다.

#### 2.3. 이미지 포맷 불일치 해결
*   **문제점**: "unrecognized layout identifier 'b8g8r8a8_unorm'" 오류 및 Validation Layer의 포맷 불일치 경고. C++ 코드의 이미지 포맷(`VK_FORMAT_B8G8R8A8_UNORM`)과 셰이더에서 선언한 포맷(`rgba8`)이 서로 달랐고, 이를 일치시키려다 잘못된 포맷 이름을 셰이더에 사용했습니다.
*   **해결책**: C++ 코드와 셰이더 양쪽 모두 표준 `RGBA` 순서를 사용하도록 통일했습니다.
*   **변경 내용**:
    1.  **`raygen.rgen`**: 이미지 선언부의 포맷을 유효한 `rgba8`로 수정했습니다.
        *   `layout(binding = 1, set = 0, b8g8r8a8_unorm) ...` -> `layout(binding = 1, set = 0, rgba8) ...`
    2.  **`Vulkan_ex01_vn.cpp`**: `chooseSwapSurfaceFormat` 함수에서 선호하는 스왑체인 포맷을 `VK_FORMAT_B8G8R8A8_UNORM`에서 `VK_FORMAT_R8G8B8A8_UNORM`으로 변경하여 셰이더와 일치시켰습니다.
        *   `if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM ...)` -> `if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM ...)`

#### 2.4. 누락된 GPU 기능 활성화
*   **문제점**: Validation Layer에서 `shaderInt64`, `scalarBlockLayout` 기능이 필요하다는 오류 발생. 셰이더는 버퍼 주소 계산을 위해 64비트 정수를, 정점(Vertex) 데이터 접근을 위해 `scalar` 메모리 레이아웃을 사용하고 있었지만, C++ 코드에서 해당 기능들을 사용하겠다고 명시적으로 활성화하지 않아 런타임 오류가 발생했습니다.
*   **해결책**: `Vulkan_ex01_vn.cpp`의 `createLogicalDevice` 함수에서 장치(Device)를 생성할 때, 필요한 기능들을 활성화하는 구조체를 추가했습니다.
*   **변경 내용**:
    *   `VkDeviceCreateInfo`를 채우기 전, `pNext` 체인에 `VkPhysicalDeviceShaderInt64Features`와 `VkPhysicalDeviceScalarBlockLayoutFeatures` 구조체를 연결하고 각각의 기능 플래그를 `VK_TRUE`로 설정했습니다.
    
        ```cpp
        // Vulkan_ex01_vn.cpp의 createLogicalDevice 함수 수정 부분
        VkPhysicalDeviceScalarBlockLayoutFeatures scalarLayoutFeatures{};
        scalarLayoutFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES;
        scalarLayoutFeatures.pNext = &asFeatures;
        scalarLayoutFeatures.scalarBlockLayout = VK_TRUE;

        VkPhysicalDeviceShaderInt64Features shaderInt64Features{};
        shaderInt64Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INT64_FEATURES;
        shaderInt64Features.pNext = &scalarLayoutFeatures;
        shaderInt64Features.shaderInt64 = VK_TRUE;

        VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
        descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
        descriptorIndexingFeatures.pNext = &shaderInt64Features;
        ```