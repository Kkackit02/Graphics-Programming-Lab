# Vulkan 예상 문제 및 답안

이 문서는 지금까지 학습한 Vulkan 자료들을 바탕으로, 실제 시험에 나올 수 있는 예상 문제와 그에 대한 상세한 모범 답안을 제공합니다. 각 문제는 Vulkan의 핵심 개념과 워크플로우를 이해하는 데 중점을 두었으며, 시험 대비에 활용하시기 바랍니다.

---

## 1. Vulkan 기본 및 초기화

### 문제 1 (개념): Vulkan이 OpenGL/DirectX와 같은 기존 API에 비해 가지는 장점과 단점을 '오버헤드'와 '제어권' 관점에서 설명하시오.

**모범 답안:**

Vulkan은 기존 API에 비해 **'낮은 오버헤드(Low Overhead)'**와 개발자에게 **'높은 제어권(Explicit Control)'**을 제공하는 것이 가장 큰 특징입니다.

*   **장점**:
    *   **낮은 오버헤드**: Vulkan은 드라이버가 자동으로 처리하던 많은 작업(메모리 관리, 동기화, 오류 검사 등)을 개발자에게 위임합니다. 이로 인해 드라이버의 부담이 줄어(오버헤드 감소), CPU 자원을 더 효율적으로 사용하고 멀티코어 CPU를 최대한 활용하여 GPU에 렌더링 명령을 더 빠르게 전달할 수 있습니다. 이는 CPU 병목 현상을 완화하여 전반적인 성능 향상으로 이어집니다.
    *   **높은 제어권**: 개발자가 GPU의 거의 모든 하드웨어 자원(메모리, 파이프라인 상태 등)을 직접 제어할 수 있습니다. 이를 통해 특정 하드웨어에 최적화된 코드를 작성하거나, 복잡한 렌더링 기법을 구현하는 데 유연성을 확보할 수 있습니다.

*   **단점**:
    *   **높은 복잡성**: 개발자가 직접 제어해야 할 부분이 많아 코드가 매우 장황하고, 학습 곡선이 가파릅니다. 작은 실수 하나가 치명적인 오류로 이어질 수 있습니다.
    *   **개발 시간 증가**: 상세한 제어는 개발 시간을 증가시키고, 디버깅을 어렵게 만듭니다.

### 문제 2 (워크플로우): Vulkan 애플리케이션이 화면에 첫 삼각형을 그리기까지의 필수적인 초기화 객체 8가지를 순서대로 나열하고, 각 객체의 역할을 한 문장으로 설명하시오.

**모범 답안:**

Vulkan 애플리케이션이 화면에 첫 삼각형을 그리기까지의 8가지 필수 초기화 객체와 역할은 다음과 같습니다.

1.  **`VkInstance`**: Vulkan API 사용을 시작하기 위한 최상위 객체로, 애플리케이션과 Vulkan 드라이버를 연결합니다.
2.  **`VkPhysicalDevice` / `VkLogicalDevice`**: 시스템의 물리 GPU를 선택하고, 해당 GPU의 특정 기능들을 활성화하여 사용하는 소프트웨어 인터페이스를 생성합니다.
3.  **`VkSurface` / `VkSwapchain`**: 렌더링 결과가 그려질 창의 표면을 생성하고, 화면에 부드러운 애니메이션을 보여주기 위한 이미지들의 큐를 준비합니다.
4.  **`VkImageView`**: 스왑체인 이미지들을 어떻게 바라보고 사용할지 정의하는 '뷰'를 생성합니다.
5.  **`VkRenderPass`**: 렌더링 작업의 전체 구조(어떤 어태치먼트를 사용하고 어떻게 처리할지)를 정의하는 '작업 계획서'를 작성합니다.
6.  **`VkPipeline`**: 렌더링의 모든 단계(셰이더, 입력 형식, 래스터화 등)를 정의하고 고정시킨 '공장 조립 라인'을 구축합니다.
7.  **`VkFramebuffer` / `VkCommandPool` / `VkCommandBuffer`**: 렌더 패스가 그림을 그릴 실제 캔버스(프레임버퍼)를 준비하고, GPU가 실행할 명령들을 기록하는 '작업 지시서'(커맨드 버퍼)를 할당합니다.
8.  **`VkQueue`**: 커맨드 버퍼를 제출하여 GPU가 작업을 수행하도록 하는 '창구'를 얻습니다.

### 문제 3 (동기화): `VkFence`와 `VkSemaphore`의 차이점과 각각의 주된 사용 목적을 설명하시오.

**모범 답안:**

`VkFence`와 `VkSemaphore`는 모두 Vulkan에서 동기화를 위해 사용되지만, 그 목적과 주체가 다릅니다.

*   **`VkFence`**:
    *   **차이점**: **CPU와 GPU 간의 동기화**에 사용됩니다.
    *   **사용 목적**: CPU가 GPU의 특정 작업 완료를 기다릴 때 사용됩니다. 예를 들어, CPU는 `vkQueueSubmit` 시 펜스를 함께 제출하고, `vkWaitForFences`를 호출하여 해당 펜스가 GPU에 의해 '시그널'될 때까지 대기합니다. 이를 통해 CPU가 GPU보다 너무 앞서나가 리소스 충돌을 일으키는 것을 방지합니다.

*   **`VkSemaphore`**:
    *   **차이점**: **GPU 내의 작업 큐 사이의 동기화**에 사용됩니다.
    *   **사용 목적**: GPU의 여러 작업(예: 이미지 획득, 렌더링, 화면 표시) 간의 순서를 보장합니다. 예를 들어, '이미지 획득' 작업이 끝나야 '렌더링' 작업이 시작되고, '렌더링'이 끝나야 '화면 표시' 작업이 시작되도록 세마포어를 통해 순서를 제어합니다.

---

## 2. 버퍼 및 디스크립터

### 문제 1 (성능): Staging Buffer를 사용하는 이유를 '메모리 타입'(`HOST_VISIBLE`, `DEVICE_LOCAL`)과 연관지어 설명하고, 데이터 전송 과정을 단계별로 서술하시오.

**모범 답안:**

**Staging Buffer를 사용하는 이유**:
GPU 메모리는 `HOST_VISIBLE`(CPU가 접근 가능)과 `DEVICE_LOCAL`(GPU가 빠르게 접근 가능) 등 다양한 타입이 있습니다. `DEVICE_LOCAL` 메모리는 GPU가 렌더링 중 가장 빠르게 접근할 수 있는 고성능 메모리이지만, CPU가 직접 접근하기 어렵거나 불가능합니다. 반면 `HOST_VISIBLE` 메모리는 CPU가 쉽게 데이터를 쓸 수 있지만, GPU 입장에서는 상대적으로 느립니다. 렌더링 루프 동안 GPU는 버텍스 버퍼를 매 프레임 읽어야 하므로, 이 버퍼는 `DEVICE_LOCAL` 메모리에 있는 것이 이상적입니다. Staging Buffer는 CPU가 데이터를 써놓을 수 있는 `HOST_VISIBLE` 메모리에 임시 버퍼를 만들고, GPU의 고속 전송 기능을 이용해 이 데이터를 `DEVICE_LOCAL` 메모리에 있는 최종 버퍼로 '단 한 번' 복사하기 위한 전략입니다. 이는 초기 설정은 복잡하지만, 렌더링 성능을 극대화하기 위한 필수적인 과정입니다.

**데이터 전송 과정 (단계별):**

1.  **Staging Buffer 생성**: `VK_BUFFER_USAGE_TRANSFER_SRC_BIT`와 `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT` 속성을 가진 임시 버퍼를 생성합니다.
2.  **CPU 데이터 복사**: `vkMapMemory`로 Staging Buffer 메모리를 CPU 주소 공간에 매핑한 후, `memcpy`를 사용하여 CPU 데이터를 이 버퍼에 복사하고 `vkUnmapMemory`로 매핑을 해제합니다.
3.  **최종 버퍼 생성**: `VK_BUFFER_USAGE_TRANSFER_DST_BIT`와 `VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT` 속성을 가진 최종 버퍼(예: Vertex Buffer)를 생성합니다.
4.  **GPU 간 복사**: 커맨드 버퍼에 `vkCmdCopyBuffer` 명령을 기록하여 Staging Buffer의 데이터를 최종 버퍼로 복사합니다. 이 명령은 GPU가 가장 효율적인 방식으로 데이터를 전송하도록 합니다.
5.  **Staging Buffer 해제**: 복사가 완료되면 Staging Buffer는 더 이상 필요 없으므로 해제합니다.

### 문제 2 (디스크립터): 셰이더에서 유니폼 버퍼를 사용하기 위해 필요한 디스크립터 관련 객체 3가지(`VkDescriptorSetLayout`, `VkDescriptorPool`, `VkDescriptorSet`)의 역할과 생성 및 바인딩까지의 워크플로우를 '도서관' 비유를 사용하여 설명하시오.

**모범 답안:**

셰이더가 유니폼 버퍼와 같은 GPU 리소스에 접근하기 위한 디스크립터 시스템은 '도서관' 비유로 설명할 수 있습니다.

1.  **`VkDescriptorSetLayout` (도서관의 '장서 분류 규칙' 만들기)**:
    *   **역할**: 파이프라인이 어떤 종류의 리소스(책)를, 몇 번 서가(binding)에서 사용할 것인지 **설계도를 정의**합니다. 셰이더가 `layout(binding = 0) uniform ...` 처럼 리소스를 선언할 때, 이 레이아웃이 그 약속을 정의합니다.
    *   **생성**: `VkDescriptorSetLayoutBinding` 구조체들을 모아 `VkDescriptorSetLayoutCreateInfo`를 통해 `vkCreateDescriptorSetLayout`으로 생성합니다.

2.  **`VkDescriptorPool` (도서관 '건물' 짓기)**:
    *   **역할**: 실제 디스크립터(책갈피)들을 할당할 수 있는 **메모리 풀**을 만듭니다. 어떤 종류의 디스크립터(예: 유니폼 버퍼용, 텍스처용)를 몇 개나 담을 수 있는지 미리 지정합니다.
    *   **생성**: `VkDescriptorPoolSize` 배열과 `VkDescriptorPoolCreateInfo`를 통해 `vkCreateDescriptorPool`으로 생성합니다.

3.  **`VkDescriptorSet` (도서관 '회원증' 발급받기)**:
    *   **역할**: 위에서 만든 풀에서, 정의된 레이아웃(분류 규칙)에 맞는 **실제 디스크립터 셋을 할당**받습니다. 이 객체가 셰이더가 참조할 리소스의 실제 주소록 역할을 합니다.
    *   **생성**: `VkDescriptorSetAllocateInfo`를 통해 `vkAllocateDescriptorSets`로 생성합니다.

**워크플로우 (바인딩까지):**

1.  **`vkUpdateDescriptorSets` 실행 ('책'을 서가에 꽂기)**: 할당된 `VkDescriptorSet`의 각 바인딩이 **실제 어떤 리소스(유니폼 버퍼, 이미지 뷰 등)를 가리킬지 연결**합니다. `VkDescriptorBufferInfo`나 `VkDescriptorImageInfo`를 사용하여 리소스 정보를 제공합니다.
2.  **`vkCmdBindDescriptorSets` 실행 (책 참고하여 작업하기)**: 렌더링 커맨드 버퍼에 "이제부터 이 디스크립터 셋(회원증)에 적힌 주소록을 참고해서 렌더링 작업을 수행해라"라고 GPU에게 명령을 기록합니다. 이 명령은 파이프라인에 디스크립터 셋을 연결하여 셰이더가 리소스에 접근할 수 있게 합니다.

---

## 3. 텍스처 및 뎁스 버퍼

### 문제 1 (워크플로우): 이미지 파일을 로드하여 3D 객체의 텍스처로 사용하기까지의 전체 과정을 서술하시오. (Staging Buffer, `VkImage`, `VkImageView`, `VkSampler`, Layout Transition 포함)

**모범 답안:**

이미지 파일을 로드하여 3D 객체의 텍스처로 사용하기까지의 전체 과정은 다음과 같습니다.

1.  **이미지 파일 로드**: `stb_image`와 같은 라이브러리를 사용하여 `.jpg`나 `.png` 같은 이미지 파일을 CPU 메모리로 로드하고, 픽셀 데이터와 이미지의 너비, 높이, 채널 수 등을 얻습니다.
2.  **Staging Buffer 생성 및 데이터 복사**: 로드된 픽셀 데이터를 GPU로 효율적으로 전송하기 위해 `HOST_VISIBLE` 메모리 속성을 가진 Staging Buffer를 생성합니다. `vkMapMemory`로 이 버퍼를 매핑한 후 `memcpy`로 픽셀 데이터를 복사하고 매핑을 해제합니다.
3.  **`VkImage` 생성**: 텍스처 데이터를 저장할 GPU 전용 `VkImage` 객체를 생성합니다. 이때 `VK_IMAGE_USAGE_TRANSFER_DST_BIT`(복사 대상)와 `VK_IMAGE_USAGE_SAMPLED_BIT`(셰이더 샘플링) 용도를 지정하고, `DEVICE_LOCAL` 메모리에 할당합니다.
4.  **이미지 레이아웃 전환 (1)**: `vkCmdPipelineBarrier`를 사용하여 `VkImage`의 레이아웃을 `VK_IMAGE_LAYOUT_UNDEFINED`에서 `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`로 전환합니다. 이는 이미지가 외부로부터 데이터를 복사받을 준비가 되었음을 GPU에 알립니다.
5.  **Staging Buffer에서 `VkImage`로 데이터 복사**: 커맨드 버퍼에 `vkCmdCopyBufferToImage` 명령을 기록하여 Staging Buffer의 픽셀 데이터를 `VkImage`로 복사합니다.
6.  **이미지 레이아웃 전환 (2)**: `vkCmdPipelineBarrier`를 사용하여 `VkImage`의 레이아웃을 `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`에서 `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`로 전환합니다. 이는 이미지가 셰이더에서 읽기 전용 텍스처로 사용될 준비가 되었음을 알립니다.
7.  **`VkImageView` 생성**: `VkImage`를 셰이더가 접근할 수 있는 형태로 정의하는 `VkImageView`를 생성합니다. 이미지의 포맷, 뷰 타입, 서브리소스 범위 등을 지정합니다.
8.  **`VkSampler` 생성**: 셰이더가 텍스처를 샘플링(색상 추출)할 때 사용할 필터링(확대/축소), 주소 모드(UV 범위 초과 시 처리) 등의 규칙을 정의하는 `VkSampler` 객체를 생성합니다.
9.  **디스크립터 셋 업데이트**: `VkDescriptorSetLayout`에 `VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER` 타입의 바인딩을 추가하고, `vkUpdateDescriptorSets`를 사용하여 생성된 `VkImageView`와 `VkSampler`를 디스크립터 셋에 연결합니다. 이제 셰이더에서 이 텍스처를 사용할 수 있습니다.

### 문제 2 (개념): 이미지 레이아웃 전환(`vkCmdPipelineBarrier` 사용)이 왜 필수적인지, `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`과 `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL` 상태를 예시로 들어 설명하시오.

**모범 답안:**

**이미지 레이아웃 전환의 필수성**:
GPU는 명령을 받으면 가능한 한 병렬로 동시에 처리하여 효율을 극대화합니다. 하지만 어떤 작업들은 반드시 순서가 지켜져야 하며, 리소스(특히 이미지)에 대한 접근 방식이 변경될 때 GPU는 해당 리소스의 메모리 배치를 최적화해야 합니다. `vkCmdPipelineBarrier`를 사용한 이미지 레이아웃 전환은 이러한 **작업 순서 보장(동기화)**과 **메모리 배치 최적화**를 위해 필수적입니다.

**예시 설명**:

1.  **`VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`**:
    *   **목적**: 이 레이아웃은 이미지가 외부 버퍼(예: Staging Buffer)로부터 픽셀 데이터를 **복사받는 대상(Destination)**으로 사용될 때 GPU가 가장 효율적으로 데이터를 쓸 수 있도록 메모리 배치를 최적화합니다.
    *   **필수성**: `vkCmdCopyBufferToImage`와 같은 복사 명령을 실행하기 전에 이미지는 반드시 이 레이아웃 상태여야 합니다. 이 상태에서는 셰이더가 이미지를 읽는 등의 다른 작업은 효율적으로 수행할 수 없습니다.

2.  **`VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`**:
    *   **목적**: 이 레이아웃은 이미지가 셰이더에서 **읽기 전용 텍스처**로 사용될 때 GPU가 가장 효율적으로 데이터를 읽을 수 있도록 메모리 배치를 최적화합니다.
    *   **필수성**: 프래그먼트 셰이더에서 `texture()` 함수를 통해 텍스처를 샘플링하기 전에 이미지는 반드시 이 레이아웃 상태여야 합니다. 이 상태에서는 이미지에 데이터를 쓰는 작업은 효율적으로 수행할 수 없습니다.

**`vkCmdPipelineBarrier`의 역할**:
`vkCmdPipelineBarrier`는 '교통정리' 신호와 같습니다. 예를 들어, '이미지에 데이터를 복사하는 작업(Transfer)'이 끝나기 전에 '셰이더가 그 이미지를 읽는 작업(Shader Read)'이 시작되면, 덜 복사된 깨진 이미지를 읽게 됩니다. 배리어를 세워주면, GPU는 이 장벽 앞의 모든 쓰기 작업이 완료되었음을 보장한 뒤에야 다음 단계의 읽기 작업을 시작합니다. `srcAccessMask`, `dstAccessMask`, `srcStageMask`, `dstStageMask` 파라미터들을 통해 어떤 작업이 끝나야 하고 어떤 작업이 시작될 수 있는지를 명시하여 정확한 동기화를 수행합니다.

---

## 4. 병렬 프로그래밍

### 문제 1 (개념): Vulkan 멀티스레드 렌더링에서 "스레드당 하나의 커맨드 풀" 패턴을 사용하는 이유를 `VkCommandPool`의 스레드 안전성(Thread-Safety)과 연관지어 설명하시오.

**모범 답안:**

**`VkCommandPool`의 스레드 안전성 문제**:
`VkCommandPool` 객체는 **스레드에 안전하지 않습니다(Not Thread-Safe)**. 이는 두 개 이상의 스레드가 **동시에** 같은 커맨드 풀을 사용하여 커맨드 버퍼를 할당(`vkAllocateCommandBuffers`)하거나 리셋하는 등의 작업을 수행하면, 내부 메모리 구조가 깨져 충돌이나 예측 불가능한 오류가 발생할 수 있음을 의미합니다.

**"스레드당 하나의 커맨드 풀" 패턴을 사용하는 이유**:
이러한 스레드 안전성 문제를 해결하기 위해, Vulkan 멀티스레드 렌더링에서는 **각 스레드마다 별도의 `VkCommandPool`을 생성하는 패턴**을 사용합니다.

1.  **경쟁 조건(Race Condition) 방지**: 각 스레드가 자신만의 커맨드 풀을 가지므로, 여러 스레드가 동시에 커맨드 버퍼를 할당하거나 리셋하려 할 때 발생하는 경쟁 조건을 원천적으로 차단합니다.
2.  **락(Lock) 오버헤드 제거**: 하나의 공유 커맨드 풀을 사용하면서 뮤텍스(Mutex) 같은 잠금 메커니즘으로 보호할 경우, 스레드들은 순차적으로 커맨드 풀에 접근해야 하므로 병렬 처리의 이점을 상실하고 락 오버헤드가 발생합니다. 스레드당 풀을 사용하면 이러한 락이 필요 없어 CPU 시간을 절약하고 진정한 병렬 실행을 가능하게 합니다.
3.  **효율성 증대**: 각 스레드는 다른 스레드와 간섭 없이 독립적으로 자신만의 커맨드 버퍼를 생성하고 기록할 수 있어, 렌더링 명령 생성 속도를 극대화할 수 있습니다.

### 문제 2 (워크플로우): Primary Command Buffer와 Secondary Command Buffer의 역할 차이를 설명하고, 멀티스레드 환경에서 이 둘을 사용하여 렌더링 명령을 생성하고 제출하는 과정을 서술하시오. (`vkCmdExecuteCommands` 포함)

**모범 답안:**

**Primary Command Buffer와 Secondary Command Buffer의 역할 차이**:

*   **Primary Command Buffer (`VK_COMMAND_BUFFER_LEVEL_PRIMARY`)**:
    *   **역할**: GPU 큐에 직접 제출될 수 있는 유일한 종류의 커맨드 버퍼입니다. 렌더 패스를 시작하고 끝낼 수 있으며, `Secondary Command Buffer`들을 실행하도록 명령할 수 있습니다. '총괄 셰프'에 비유할 수 있습니다.
*   **Secondary Command Buffer (`VK_COMMAND_BUFFER_LEVEL_SECONDARY`)**:
    *   **역할**: 독립적으로 GPU 큐에 제출될 수 없으며, 반드시 `Primary Command Buffer` 내에서 `vkCmdExecuteCommands`를 통해 실행되어야 합니다. 렌더 패스 내에서 실행될 명령 묶음을 기록하는 데 특화되어 있으며, 병렬로 생성하기에 이상적입니다. '부주방장'에 비유할 수 있습니다.

**멀티스레드 렌더링 워크플로우**:

1.  **메인 스레드 (총괄 셰프) - 작업 준비**:
    *   `vkBeginCommandBuffer`로 `Primary Command Buffer` 기록을 시작합니다.
    *   `vkCmdBeginRenderPass`로 렌더 패스를 시작합니다. 이때 `contents` 파라미터를 `VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS`로 설정하여, 이 렌더 패스의 내용은 `Secondary Command Buffer`들로 채워질 것임을 명시합니다.
    *   렌더링할 객체들을 여러 워커 스레드에 분배하고, 각 스레드가 `Secondary Command Buffer`를 생성하도록 작업을 요청합니다.

2.  **워커 스레드 (부주방장들) - 병렬 명령 기록**:
    *   각 워커 스레드는 자신만의 `VkCommandPool`을 사용합니다.
    *   `VkCommandBufferInheritanceInfo`를 설정하여, 자신이 어떤 렌더 패스와 프레임버퍼 컨텍스트 내에서 실행될 것인지 정보를 받습니다.
    *   `vkBeginCommandBuffer`를 호출(상속 정보 포함)하고, 담당할 객체들에 대한 렌더링 명령(`vkCmdBindPipeline`, `vkCmdDraw` 등)을 `Secondary Command Buffer`에 기록합니다.
    *   `vkEndCommandBuffer`로 기록을 완료합니다.

3.  **메인 스레드 (총괄 셰프) - 취합 및 제출**:
    *   모든 워커 스레드가 `Secondary Command Buffer` 기록을 마칠 때까지 기다립니다.
    *   `Primary Command Buffer`에 `vkCmdExecuteCommands`를 기록하여, 워커 스레드들이 생성한 모든 `Secondary Command Buffer`들을 실행하도록 지시합니다.
    *   `vkCmdEndRenderPass`로 렌더 패스를 종료하고, `vkEndCommandBuffer`로 `Primary Command Buffer` 기록을 마칩니다.
    *   완성된 `Primary Command Buffer`를 `vkQueueSubmit`으로 GPU에 제출합니다.

---

## 5. 레이 트레이싱

### 문제 1 (개념): 레이 트레이싱의 핵심 구성요소 3가지(가속 구조, 레이 트레이싱 파이프라인, SBT)의 역할을 각각 설명하시오.

**모범 답안:**

레이 트레이싱의 3가지 핵심 구성요소와 그 역할은 다음과 같습니다.

1.  **가속 구조 (Acceleration Structure - AS)**:
    *   **역할**: 레이 트레이싱에서 광선과 씬의 기하학적 객체(삼각형) 간의 교차 테스트 속도를 획기적으로 높이기 위한 계층적 자료 구조입니다. 광선이 모든 객체와 일일이 교차 테스트를 수행하는 비효율을 줄여줍니다.
    *   **종류**: `BLAS`(Bottom-Level AS, 단일 객체), `TLAS`(Top-Level AS, 씬 전체)로 구성됩니다.

2.  **레이 트레이싱 파이프라인 (Ray Tracing Pipeline)**:
    *   **역할**: 래스터화 파이프라인과 달리, 광선 추적에 특화된 새로운 셰이더들(Ray Generation, Closest Hit, Miss 등)을 그룹으로 묶어 관리하고 실행하는 파이프라인입니다. 이벤트 기반으로 셰이더가 호출되는 동적인 특성을 가집니다.

3.  **셰이더 바인딩 테이블 (Shader Binding Table - SBT)**:
    *   **역할**: GPU 메모리에 있는 테이블로, 레이 트레이싱 파이프라인에 있는 셰이더 그룹들의 핸들(주소)을 저장합니다. 광선 추적 중 특정 이벤트(광선 생성, 교차, 누락)가 발생했을 때, GPU가 SBT를 참조하여 어떤 셰이더 코드를 실행할지 결정하는 '이벤트 대응 매뉴얼' 역할을 합니다.

### 문제 2 (데이터 흐름): Ray Generation 셰이더에서 `traceRayEXT`가 호출된 후, 광선이 물체와 부딪혔을 때(Hit)와 부딪히지 않았을 때(Miss) 각각 어떤 셰이더가 호출되며, 셰이더 간의 데이터 전달(Payload)은 어떻게 이루어지는지 설명하시오.

**모범 답안:**

Ray Generation 셰이더에서 `traceRayEXT`가 호출되면 광선 추적 여정이 시작되며, 그 결과에 따라 다음과 같은 셰이더 호출과 데이터 전달이 이루어집니다.

1.  **광선이 물체와 부딪혔을 때 (Hit)**:
    *   **호출 셰이더**: 광선이 씬 내의 물체와 교차하고, 그중 **가장 가까운 교차점**이 발견되면 해당 물체에 연결된 **Closest Hit 셰이더**가 호출됩니다.
    *   **역할**: Closest Hit 셰이더는 교차점의 재질, 법선 벡터, 조명 정보 등을 사용하여 최종 색상을 계산합니다.
    *   **데이터 전달 (Payload)**: 계산된 색상 값은 **페이로드(Payload)**라는 특별한 변수(예: `layout(location = 0) rayPayloadInEXT vec3 pld;`)에 기록됩니다. 이 페이로드는 광선 여행자의 '여행 일지'와 같으며, Ray Generation 셰이더에서 `traceRayEXT` 호출 전에 초기화되고, Closest Hit 셰이더에서 업데이트됩니다.

2.  **광선이 물체와 부딪히지 않았을 때 (Miss)**:
    *   **호출 셰이더**: 광선이 씬 내의 어떤 물체와도 교차하지 않고 '허공'으로 사라지면 **Miss 셰이더**가 호출됩니다.
    *   **역할**: Miss 셰이더는 보통 씬의 배경색이나 스카이박스 색상과 같은 기본 값을 반환합니다.
    *   **데이터 전달 (Payload)**: Miss 셰이더 역시 계산된 배경색을 페이로드 변수에 기록합니다.

**페이로드(Payload)를 통한 데이터 전달**:
`traceRayEXT` 함수 호출이 끝나면, Ray Generation 셰이더는 페이로드 변수에 기록된 최종 값을 읽어옵니다. 이 값은 광선이 물체에 부딪혔든(Closest Hit 셰이더가 기록), 부딪히지 않았든(Miss 셰이더가 기록) 상관없이, 해당 광선 추적의 최종 결과(색상)를 담고 있습니다. Ray Generation 셰이더는 이 페이로드 값을 사용하여 최종 픽셀의 색상을 결정하고 스토리지 이미지에 저장합니다.

---

## 6. Vulkan 파이프라인

### 문제 1 (개념): Vulkan 그래픽스 파이프라인에서 '동적 상태(Dynamic State)'가 필요한 이유와 그 활용 방법을 설명하시오.

**모범 답안:**

**'동적 상태(Dynamic State)'가 필요한 이유**:
Vulkan 파이프라인은 성능 최적화를 위해 대부분의 렌더링 상태를 파이프라인 생성 시 '고정(baked)'시킵니다. 하지만 뷰포트(Viewport) 크기나 가위(Scissor) 영역처럼, 창 크기 변경이나 UI 요소 배치에 따라 렌더링 중에 자주 변경되어야 하는 상태들이 있습니다. 만약 이러한 상태들까지 고정되어 있다면, 상태가 변경될 때마다 비싼 파이프라인 객체 전체를 다시 생성해야 하므로 매우 비효율적입니다. '동적 상태'는 이러한 특정 상태들을 파이프라인에서 제외하고, 렌더링 중에 커맨드 버퍼를 통해 동적으로 변경할 수 있도록 허용하여 성능과 유연성 사이의 균형을 제공합니다.

**활용 방법**:

1.  **파이프라인 생성 시 지정**: `VkPipelineDynamicStateCreateInfo` 구조체에 동적으로 변경할 상태(예: `VK_DYNAMIC_STATE_VIEWPORT`, `VK_DYNAMIC_STATE_SCISSOR`) 목록을 지정하여 파이프라인을 생성합니다.
2.  **커맨드 버퍼 기록 시 변경**: 이렇게 생성된 파이프라인은 커맨드 버퍼 기록 시 `vkCmdSetViewport`, `vkCmdSetScissor` 같은 `vkCmdSet...` 함수들을 사용하여 해당 상태를 실시간으로 변경할 수 있습니다. 이 함수들은 파이프라인 객체 자체를 다시 만들 필요 없이 GPU의 상태를 업데이트합니다.

### 문제 2 (개념): Vulkan에서 '파이프라인 캐시(Pipeline Cache)'가 필요한 이유와 작동 방식을 설명하시오.

**모범 답안:**

**'파이프라인 캐시(Pipeline Cache)'가 필요한 이유**:
Vulkan에서 파이프라인 객체(`VkPipeline`)를 생성하는 것은 셰이더 컴파일, GPU 하드웨어에 맞는 최적화 등 매우 복잡하고 시간이 오래 걸리는(비싼) 작업입니다. 애플리케이션이 시작될 때마다 모든 파이프라인을 새로 생성하면 로딩 시간이 길어지게 됩니다. 파이프라인 캐시는 이러한 파이프라인 생성 비용을 줄여 애플리케이션의 로딩 시간을 단축하기 위해 필요합니다.

**작동 방식**:

1.  **캐시 생성**: `vkCreatePipelineCache` 함수를 통해 `VkPipelineCache` 객체를 생성합니다.
2.  **파이프라인 생성 시 활용**: `vkCreateGraphicsPipelines` 또는 `vkCreateRayTracingPipelinesKHR` 함수를 호출할 때, 이 `VkPipelineCache` 객체를 파라미터로 전달합니다. 드라이버는 이 캐시를 사용하여 파이프라인 컴파일 결과를 저장하거나, 이미 저장된 결과를 재사용하여 파이프라인 생성 속도를 높입니다.
3.  **캐시 데이터 저장 및 로드**: 애플리케이션 종료 전에 `vkGetPipelineCacheData`를 호출하여 캐시 데이터를 바이너리 형태로 추출하고 파일에 저장할 수 있습니다. 다음 애플리케이션 실행 시, 이 파일에서 캐시 데이터를 읽어 `VkPipelineCache`를 초기화하면, 이전에 생성했던 파이프라인들을 훨씬 빠르게 다시 만들 수 있습니다.

---
