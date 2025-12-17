# Vulkan 학습 자료집: 04. 병렬 프로그래밍 (Parallel Programming)

이 문서는 `수정_04Vulkan (2).pdf` 파일의 내용을 바탕으로, Vulkan의 핵심적인 성능 최적화 기법인 **병렬 명령 생성(Parallel Command Generation)**에 대해 상세히 설명합니다. 멀티코어 CPU를 활용하여 렌더링 성능을 극대화하는 원리와 구현 방법을 이해하고 시험에 대비하는 것을 목표로 합니다.

---

### 1. 학습 목표

- **Vulkan 병렬 프로그래밍의 이해**: 왜 Vulkan에서 병렬 처리가 중요하며, OpenGL과 같은 기존 API와 어떻게 다른지 이해합니다.
- **커맨드 풀과 스레드 안전성**: `VkCommandPool`이 스레드에 안전하지 않다는 점을 인지하고, 멀티스레드 환경에서 이를 어떻게 다루어야 하는지 학습합니다.
- **기본/보조 커맨드 버퍼**: `Primary` 및 `Secondary` 커맨드 버퍼의 역할과 차이점을 명확히 구분하고, 병렬 렌더링에서의 활용법을 익힙니다.
- **멀티스레드 렌더링 워크플로우**: 여러 스레드를 사용하여 렌더링 명령을 동시에 생성하고, 이를 취합하여 GPU에 제출하는 전체 과정을 학습합니다.

---

### 2. 주요 개념 상세 설명

#### 2.1. 왜 Vulkan에서 병렬 프로그래밍이 중요한가?

- **CPU 병목 현상**: 최신 GPU는 수천 개의 코어를 가진 병렬 처리 괴물이지만, 정작 GPU가 무슨 일을 할지 지시하는 것은 CPU입니다. 복잡한 씬에서는 CPU가 렌더링 명령(Draw Call)을 생성하는 속도가 GPU가 처리하는 속도를 따라가지 못하는 **CPU 병목(CPU-bound)** 현상이 발생합니다.
- **Vulkan의 설계 철학**: Vulkan은 이러한 문제를 해결하기 위해 설계 단계부터 멀티스레딩을 염두에 두었습니다. 드라이버의 부담을 최소화하고 개발자가 직접 여러 CPU 코어를 사용하여 렌더링 명령을 병렬로 준비할 수 있도록 허용합니다. 이는 단일 스레드 모델에 의존하는 경향이 있는 OpenGL과의 근본적인 차이점입니다.

#### 2.2. 커맨드 풀 (`VkCommandPool`)과 스레드 안전성

- **핵심 문제**: `VkCommandPool` 객체는 **스레드에 안전하지 않습니다(Not Thread-Safe)**.
- **의미**: 두 개 이상의 스레드가 **동시에** 같은 커맨드 풀을 사용하여 커맨드 버퍼를 할당(`vkAllocateCommandBuffers`)하거나 리셋하는 등의 작업을 수행하면, 내부 메모리 구조가 깨져 충돌이나 예측 불가능한 오류가 발생할 수 있습니다.
- **잘못된 해결책**: 하나의 커맨드 풀을 여러 스레드에서 공유하면서 뮤텍스(Mutex) 같은 잠금(Lock) 메커니즘으로 보호할 수 있습니다. 하지만 이는 스레드들이 순차적으로 커맨드 풀에 접근하게 만들어 병렬 처리의 이점을 완전히 상실시킵니다.
- **올바른 해결책**: **각 스레드마다 별도의 `VkCommandPool`을 생성합니다.** 이렇게 하면 각 스레드는 다른 스레드와 간섭 없이 독립적으로 자신만의 커맨드 버퍼를 생성하고 기록할 수 있습니다. 이것이 Vulkan 멀티스레딩의 핵심 패턴입니다.

![Command Pool Creation](https://github.com/Project-Burn/asset_storage/blob/main/vulkan_tutorial/04_Parallel_Programming_in_Vulkan/12.png?raw=true)
*(이미지 설명: Frame N-2, N-1, N에 대해 Thread 1과 Thread 2가 각각 자신만의 CommandPool을 가지고 독립적으로 Command Buffer를 생성하는 모습을 보여줍니다.)*

#### 2.3. 기본(Primary) vs. 보조(Secondary) 커맨드 버퍼

Vulkan은 두 가지 레벨의 커맨드 버퍼를 제공합니다.

1.  **기본 커맨드 버퍼 (`VK_COMMAND_BUFFER_LEVEL_PRIMARY`)**
    - **역할**: GPU 큐에 직접 제출될 수 있는 유일한 종류의 커맨드 버퍼입니다.
    - **특징**: 렌더 패스를 시작(`vkCmdBeginRenderPass`)하고 끝낼(`vkCmdEndRenderPass`) 수 있습니다. 다른 보조 커맨드 버퍼들을 실행하라는 명령(`vkCmdExecuteCommands`)을 담을 수 있습니다.
    - **비유**: 오케스트라의 **총보(Full Score)** 또는 연극의 **메인 대본**. 전체 흐름을 지휘합니다.

2.  **보조 커맨드 버퍼 (`VK_COMMAND_BUFFER_LEVEL_SECONDARY`)**
    - **역할**: 독립적으로 GPU 큐에 제출될 수 없으며, 반드시 기본 커맨드 버퍼 내에서 실행되어야 합니다.
    - **특징**: 렌더 패스 내에서 실행될 명령 묶음을 기록하는 데 특화되어 있습니다. 병렬로 생성하기에 이상적입니다.
    - **비유**: 오케스트라의 **파트보(Part Score)** (바이올린 파트, 첼로 파트 등) 또는 연극의 특정 **장면(Scene)**. 여러 파트가 동시에 준비될 수 있습니다.

#### 2.4. 멀티스레드 렌더링 워크플로우

1.  **메인 스레드 - 준비 단계**
    - **기본 커맨드 버퍼 기록 시작**: `vkBeginCommandBuffer`로 기본 커맨드 버퍼 기록을 시작합니다.
    - **렌더 패스 시작**: `vkCmdBeginRenderPass`를 호출합니다. 이때 `contents` 파라미터를 `VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS`로 설정하여, 이 렌더 패스의 내용은 보조 커맨드 버퍼들로 채워질 것임을 명시합니다.
    - **작업 분배**: 렌더링할 객체들을 여러 스레드에 분배하고, 각 스레드가 보조 커맨드 버퍼를 생성하도록 작업을 요청합니다. (보통 스레드 풀 사용)

2.  **워커 스레드 - 병렬 명령 기록 단계**
    - 각 워커 스레드는 자신만의 `VkCommandPool`을 사용합니다.
    - **`VkCommandBufferInheritanceInfo` 설정**: 보조 커맨드 버퍼가 어떤 렌더 패스와 프레임버퍼의 컨텍스트 내에서 실행될 것인지에 대한 "상속 정보"를 설정합니다. 이는 드라이버가 보조 커맨드 버퍼의 명령을 올바르게 검증하고 최적화하는 데 필수적입니다.
    - **보조 커맨드 버퍼 기록**: `vkBeginCommandBuffer`를 호출(상속 정보 포함)하고, 담당할 객체들에 대한 렌더링 명령(`vkCmdBindPipeline`, `vkCmdBindDescriptorSets`, `vkCmdDraw` 등)을 기록합니다.
    - `vkEndCommandBuffer`로 기록을 완료합니다.

3.  **메인 스레드 - 취합 및 제출 단계**
    - **스레드 작업 완료 대기**: 모든 워커 스레드가 보조 커맨드 버퍼 기록을 마칠 때까지 기다립니다.
    - **결과 취합**: 각 스레드가 생성한 보조 커맨드 버퍼들의 목록을 하나의 벡터(vector)에 모읍니다.
    - **보조 커맨드 버퍼 실행**: 기본 커맨드 버퍼에 `vkCmdExecuteCommands`를 기록하여, 취합된 모든 보조 커맨드 버퍼를 실행하도록 지시합니다.
    - **렌더 패스 종료**: `vkCmdEndRenderPass`를 호출합니다.
    - **기본 커맨드 버퍼 기록 종료**: `vkEndCommandBuffer`로 기록을 마칩니다.
    - **큐 제출**: 완성된 기본 커맨드 버퍼를 `vkQueueSubmit`으로 GPU에 제출합니다.

![Multithreading Workflow](https://github.com/Project-Burn/asset_storage/blob/main/vulkan_tutorial/04_Parallel_Programming_in_Vulkan/20.png?raw=true)
*(이미지 설명: 메인 스레드가 작업을 조율하고, 여러 워커 스레드(Thread 2, 4)가 병렬로 Cmd Buffer를 생성하여 메인 스레드에 다시 전달하는 과정을 보여줍니다.)*

---

### 3. 시험 대비 핵심 요약

- **Vulkan 병렬 처리의 핵심 원칙**:
    - **"스레드당 하나의 커맨드 풀 (One Command Pool per Thread)"**: 스레드 간의 동기화 비용을 없애고 진정한 병렬 실행을 가능하게 하는 가장 중요한 패턴입니다.
- **커맨드 버퍼의 역할 분담**:
    - **Primary**: 전체 렌더링 흐름(렌더 패스 시작/종료)을 제어하고, Secondary 버퍼들을 "실행"시키는 컨테이너 역할. **큐에 직접 제출됩니다.**
    - **Secondary**: 실제 렌더링 명령(Draw Call 등)의 묶음. 병렬로 생성되며, **Primary에 의해 실행됩니다.**
- **주요 API 및 구조체**:
    - `VkCommandPoolCreateInfo`: 커맨드 풀 생성 정보.
    - `VkCommandBufferAllocateInfo`: 커맨드 버퍼 할당 정보. `level` 멤버를 `VK_COMMAND_BUFFER_LEVEL_PRIMARY` 또는 `SECONDARY`로 설정.
    - `VkCommandBufferBeginInfo`: 커맨드 버퍼 기록 시작 정보.
    - `VkCommandBufferInheritanceInfo`: **(Secondary 전용)** 부모(Primary)로부터 어떤 상태(렌더 패스 등)를 상속받을지 명시.
    - `vkCmdExecuteCommands`: **(Primary 전용)** 여러 Secondary 커맨드 버퍼를 실행하도록 명령.
    - `vkCmdBeginRenderPass`의 `contents` 파라미터:
        - `VK_SUBPASS_CONTENTS_INLINE`: 렌더 패스 명령이 Primary 버퍼에 직접 기록됨 (기본).
        - `VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS`: 렌더 패스 명령이 Secondary 버퍼들로 제공됨 (병렬 처리 시).

이 자료를 통해 멀티스레드 환경에서 Vulkan의 렌더링 명령이 어떻게 효율적으로 구성되고 처리되는지 이해할 수 있습니다. 시험에서 병렬 처리나 성능 최적화 관련 문제가 나올 경우, 이 워크플로우와 각 컴포넌트의 역할을 참고하여 답안을 작성할 수 있습니다.
