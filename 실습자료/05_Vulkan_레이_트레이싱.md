# Vulkan 학습 자료집: 05. 레이 트레이싱 (Ray Tracing)

이 문서는 `수정_05Vulkan_new (1).pdf` 파일과 `Vulkan_ex01_vn.cpp` 예제 코드를 바탕으로, Vulkan의 **레이 트레이싱(Ray Tracing)** 기능에 대해 심층적으로 분석하고 설명합니다. 시험에서 복잡한 레이 트레이싱 관련 질문에 답하고, 전체 구현 흐름을 파악하는 데 도움이 되도록 구성되었습니다.

---

### 1. 학습 목표

- **레이 트레이싱의 기본 원리**: 기존의 래스터화(Rasterization) 방식과 레이 트레이싱 방식의 차이점을 이해합니다.
- **Vulkan 레이 트레이싱 핵심 구성 요소**:
    - **가속 구조 (Acceleration Structures - AS)**: `BLAS`와 `TLAS`의 개념과 역할을 학습합니다.
    - **레이 트레이싱 파이프라인**: 레이 트레이싱 전용 파이프라인과 새로운 셰이더(RayGen, Closest-Hit, Miss)들의 기능을 이해합니다.
    - **셰이더 바인딩 테이블 (Shader Binding Table - SBT)**: GPU가 어떤 셰이더를 호출해야 할지 알려주는 테이블의 원리를 배웁니다.
- **실습**: 예제 코드를 통해 실제 Vulkan 레이 트레이싱 렌더러가 어떻게 초기화되고, 장면을 구성하며, 렌더링을 수행하는지 전체 과정을 이해합니다.

---

### 2. 주요 개념 상세 설명

#### 2.1. 레이 트레이싱이란?

기존의 래스터화 방식이 3D 모델을 2D 화면에 투영하여 픽셀로 채우는 방식이라면, 레이 트레이싱은 반대로 화면의 각 픽셀에서 가상의 광선(Ray)을 쏘아 씬(Scene) 내부로 보내는 방식입니다. 광선이 물체와 부딪히면, 그 지점의 재질, 조명, 그림자 등을 계산하여 픽셀의 색상을 결정합니다. 이 방식은 반사, 굴절, 부드러운 그림자 등 사실적인 광학 효과를 표현하는 데 매우 강력합니다.

#### 2.2. Vulkan 레이 트레이싱 활성화

레이 트레이싱은 Vulkan의 핵심 기능이 아닌 **확장(Extension)** 기능입니다. 따라서 이를 사용하려면 인스턴스 및 디바이스 생성 시 관련 확장을 명시적으로 활성화해야 합니다.

- **필수 장치 확장 (Device Extensions)**:
    - `VK_KHR_acceleration_structure`: 가속 구조를 생성하고 관리하는 기능.
    - `VK_KHR_ray_tracing_pipeline`: 레이 트레이싱 파이프라인과 셰이더를 생성하는 기능.
    - `VK_KHR_buffer_device_address`: 버퍼의 GPU 메모리 주소를 직접 얻어오는 기능. 가속 구조와 SBT에서 필수적으로 사용됩니다.
    - `VK_KHR_deferred_host_operations`: 가속 구조나 파이프라인 같은 무거운 객체의 생성을 백그라운드 스레드에서 처리하는 기능.
    - `VK_EXT_descriptor_indexing`: 셰이더에서 디스크립터 배열을 동적으로 인덱싱하는 기능.

#### 2.3. 가속 구조 (Acceleration Structure - AS)

레이 트레이싱은 수많은 광선과 씬의 모든 삼각형 간의 교차 테스트를 수행해야 하므로 연산량이 엄청납니다. 가속 구조는 이 교차 테스트의 범위를 효율적으로 좁혀주는 핵심적인 자료 구조입니다. (일반적으로 BVH - Bounding Volume Hierarchy 형태로 구현됩니다.)

Vulkan은 2단계 계층 구조를 사용합니다.

1.  **BLAS (Bottom-Level Acceleration Structure)**
    - **역할**: 단일 메쉬(Mesh), 즉 하나의 객체에 대한 기하학적 정보(정점, 인덱스)를 담고 있는 가속 구조입니다.
    - **생성**: 객체의 정점 및 인덱스 버퍼로부터 생성됩니다.
    - **특징**: 한 번 생성되면 객체의 모양이 변하지 않는 한 재사용할 수 있습니다. 정적(Static) 객체에 매우 효율적입니다.

2.  **TLAS (Top-Level Acceleration Structure)**
    - **역할**: 전체 씬(Scene)을 표현하는 가속 구조입니다. 여러 개의 BLAS **인스턴스(Instance)** 들로 구성됩니다.
    - **생성**: 각 BLAS에 대한 참조와, 해당 인스턴스의 변환 행렬(위치, 회전, 크기), 인스턴스 ID 등을 담은 배열로부터 생성됩니다.
    - **특징**: 씬에 있는 객체가 움직이거나 회전할 때, BLAS 자체를 다시 만들 필요 없이 TLAS에서 해당 인스턴스의 변환 행렬만 업데이트하면 되므로 매우 효율적입니다.

![AS Hierarchy](https://github.com/Project-Burn/asset_storage/blob/main/vulkan_tutorial/05_Ray_Tracing_in_Vulkan/07.png?raw=true)
*(이미지 설명: TLAS가 여러 BLAS 인스턴스를 포함하고, 각 BLAS는 실제 정점/인덱스 버퍼로부터 생성되는 계층적 구조를 보여줍니다.)*

#### 2.4. 레이 트레이싱 파이프라인과 셰이더

레이 트레이싱은 기존의 그래픽스 파이프라인과 다른, 새로운 **레이 트레이싱 파이프라인**을 사용합니다. 이 파이프라인은 여러 종류의 셰이더들을 **셰이더 그룹(Shader Group)**으로 묶어 관리합니다.

- **셰이더 종류 및 역할**:
    1.  **Ray Generation Shader (`.rgen`)**: **진입점**. 렌더링의 시작점으로, 보통 화면의 픽셀 하나당 하나씩 실행됩니다. 광선의 원점과 방향을 계산하고 `traceRayEXT()` 함수를 호출하여 광선 추적을 시작합니다.
    2.  **Closest Hit Shader (`.rchit`)**: 광선이 여러 물체와 교차했을 때, **가장 가까운** 교차점에서 호출되는 셰이더입니다. 물체의 재질, 조명, 그림자 계산 등 실제 **셰이딩(Shading)**을 수행하고 최종 색상 값을 결정합니다.
    3.  **Miss Shader (`.rmiss`)**: 광선이 **아무 물체와도 부딪히지 않았을 때** 호출됩니다. 보통 배경색이나 스카이박스(하늘) 색상을 반환합니다. 그림자 광선을 위한 별도의 Miss 셰이더를 두기도 합니다.
    4.  **Intersection Shader (`.rint`)**: (선택 사항) 삼각형이 아닌 절차적(procedural) 기하 도형(예: 구)과의 교차 테스트를 직접 정의할 때 사용됩니다.

- **셰이더 그룹 (Shader Group)**:
    - **RayGen Group**: Ray Generation 셰이더 하나로 구성.
    - **Miss Group**: Miss 셰이더 하나로 구성.
    - **Hit Group**: Closest Hit, Any Hit, Intersection 셰이더의 조합으로 구성. 삼각형 메쉬만 사용하는 경우 보통 Closest Hit 셰이더 하나만 포함합니다.

#### 2.5. 셰이더 바인딩 테이블 (Shader Binding Table - SBT)

- **역할**: SBT는 GPU 메모리에 있는 테이블로, 레이 트레이싱 파이프라인에 있는 셰이더 그룹들의 **핸들(주소)**을 저장합니다. GPU는 광선 추적 중 특정 이벤트(광선 생성, 교차, 누락)가 발생했을 때, SBT를 참조하여 어떤 셰이더 코드를 실행할지 결정합니다.
- **구조**: SBT는 보통 세 부분으로 나뉩니다.
    1.  RayGen 셰이더 그룹 핸들
    2.  Miss 셰이더 그룹 핸들 배열
    3.  Hit 셰이더 그룹 핸들 배열
- **생성 과정**:
    1.  `vkGetRayTracingShaderGroupHandlesKHR` 함수로 파이프라인에서 셰이더 그룹 핸들을 가져옵니다.
    2.  이 핸들들을 GPU 버퍼(SBT 버퍼)에 복사합니다.
- **`vkCmdTraceRaysKHR`에서의 사용**: 이 함수를 호출할 때, SBT 버퍼의 각 섹션(RayGen, Miss, Hit) 시작 주소와 간격(stride)을 전달합니다.

#### 2.6. 레이 트레이싱 렌더링 과정 (`vkCmdTraceRaysKHR`)

1.  **커맨드 버퍼 기록 시작**:
2.  **디스크립터 셋 바인딩**: 셰이더가 필요로 하는 리소스들을 바인딩합니다.
    - `binding 0`: **TLAS** (`VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR`)
    - `binding 1`: **결과를 저장할 이미지** (`VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`)
    - `binding 2`: 카메라, 조명 정보가 담긴 **유니폼 버퍼**
3.  **레이 트레이싱 파이프라인 바인딩**: `vkCmdBindPipeline`으로 생성된 레이 트레이싱 파이프라인을 바인딩합니다.
4.  **레이 트레이싱 실행**: `vkCmdTraceRaysKHR()` 함수를 호출합니다.
    - 이 함수는 SBT의 RayGen 섹션에서 셰이더를 찾아 실행합니다.
    - RayGen 셰이더 내의 `traceRayEXT()`가 호출되면, GPU는 TLAS를 순회하며 광선과 객체의 교차를 테스트합니다.
    - 교차가 발생하면 SBT의 Hit 섹션에서, 발생하지 않으면 Miss 섹션에서 해당하는 셰이더를 찾아 실행합니다.
    - 모든 셰이딩 결과는 페이로드(payload) 변수를 통해 RayGen 셰이더로 반환되고, 최종적으로 스토리지 이미지에 기록됩니다.
5.  **결과 복사**: 레이 트레이싱 결과가 담긴 스토리지 이미지를 화면에 표시될 스왑체인 이미지로 복사합니다.

---

### 3. `Vulkan_ex01_vn.cpp` 코드 분석

- **`initVulkan()`**: 레이 트레이싱 확장을 활성화하고, `createBottomLevelAS`, `createTopLevelAS`, `createRTPipeline`, `createShaderBindingTable` 등의 초기화 함수를 순서대로 호출합니다.
- **`createBottomLevelAS()`**:
    - `loadGeometry`를 통해 `.obj` 파일에서 정점/인덱스 데이터를 읽습니다.
    - `VkAccelerationStructureGeometryKHR` 구조체에 삼각형 데이터를 정의합니다.
    - `vkCmdBuildAccelerationStructuresKHR`를 호출하여 BLAS를 빌드합니다.
- **`createTopLevelAS()`**:
    - 씬에 있는 각 객체에 대해 `VkAccelerationStructureInstanceKHR`를 생성합니다. 이 구조체에는 객체의 변환 행렬, BLAS 참조 주소, 커스텀 인덱스 등이 포함됩니다.
    - 이 인스턴스 배열로부터 `vkCmdBuildAccelerationStructuresKHR`를 호출하여 TLAS를 빌드합니다.
- **`createRTPipeline()`**:
    - `.rgen`, `.rchit`, `.rmiss` 셰이더 코드를 파일에서 읽어 `VkShaderModule`을 생성합니다.
    - 각 셰이더를 `VkRayTracingShaderGroupCreateInfoKHR`를 통해 셰이더 그룹으로 정의합니다.
    - `vkCreateRayTracingPipelinesKHR`를 호출하여 파이프라인을 생성합니다.
- **`createShaderBindingTable()`**:
    - `vkGetRayTracingShaderGroupHandlesKHR`로 셰이더 그룹 핸들을 가져옵니다.
    - RayGen, Miss, Hit 그룹별로 별도의 버퍼를 생성하고 핸들 데이터를 `memcpy`합니다.
- **`recordCommandBuffer()`**:
    - `vkCmdBindPipeline`으로 레이 트레이싱 파이프라인을 바인딩합니다.
    - `vkCmdBindDescriptorSets`로 TLAS, 스토리지 이미지, 유니폼 버퍼를 바인딩합니다.
    - `vkCmdTraceRaysKHR`를 호출하여 레이 트레이싱을 시작합니다.
    - 결과가 저장된 스토리지 이미지를 스왑체인 이미지로 복사하는 배리어를 설정하고 `vkCmdCopyImage`를 호출합니다.

---

### 4. 시험 대비 핵심 요약

- **레이 트레이싱의 3대 구성 요소**:
    1.  **가속 구조 (AS)**: 광선-객체 교차 테스트를 가속. (BLAS: 개별 객체, TLAS: 전체 씬)
    2.  **레이 트레이싱 파이프라인**: 새로운 셰이더 타입(rgen, rchit, rmiss)을 그룹으로 묶어 관리.
    3.  **셰이더 바인딩 테이블 (SBT)**: GPU가 이벤트에 맞는 셰이더를 찾아갈 수 있도록 하는 주소록.
- **핵심 실행 함수**: `vkCmdTraceRaysKHR()`
- **데이터 흐름**:
    - **입력**: TLAS, 카메라/조명 정보(UBO).
    - **처리**: RayGen -> `traceRayEXT()` -> (교차 시) ClosestHit / (비교차 시) Miss -> 결과가 RayGen으로 반환.
    - **출력**: 스토리지 이미지 (`VkImage` with `VK_IMAGE_USAGE_STORAGE_BIT`).
- **계층 구조**:
    - **장면**: TLAS는 여러 **BLAS 인스턴스**를 가짐.
    - **객체**: BLAS는 하나의 **메쉬(정점/인덱스 버퍼)**로부터 생성됨.
    - **셰이더**: 파이프라인은 여러 **셰이더 그룹**을 가짐. SBT는 이 그룹들의 **핸들**을 가짐.

이 문서는 Vulkan 레이 트레이싱의 복잡한 개념들을 단계별로 나누어 설명했습니다. 시험 시 각 구성 요소의 역할과 이들이 어떻게 상호작용하여 최종 이미지를 만들어내는지에 집중하여 참고하시면 큰 도움이 될 것입니다.
