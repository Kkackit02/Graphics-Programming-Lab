# Vulkan 학습 자료집: 02. 버퍼 관리 (Buffers)

이 문서는 `수정_02Vulkan (1).pdf` 파일의 내용을 바탕으로, Vulkan에서 데이터를 관리하는 핵심 요소인 **버퍼(Buffer)**와 **디스크립터(Descriptor)**에 대해 상세히 설명합니다. 회전하는 사각형 예제를 통해 개념을 실습하고 시험에 대비할 수 있도록 구성되었습니다.

---

### 1. 학습 목표

- **버텍스 버퍼 (Vertex Buffer)**: 정점의 위치, 색상 등 고유 데이터를 저장하는 버퍼의 기능과 생성 방법을 배웁니다.
- **유니폼 버퍼 (Uniform Buffer)**: 모든 정점에 동일하게 적용되는 전역 데이터(시간, 변환 행렬 등)를 저장하는 버퍼의 역할과 사용법을 익힙니다.
- **디스크립터 (Descriptor)**: 생성된 버퍼를 그래픽스 파이프라인의 셰이더에서 사용할 수 있도록 연결하는 메커니즘을 이해합니다.
- **실습**: 유니폼 버퍼를 매 프레임 업데이트하여 회전하는 사각형을 구현합니다.

---

### 2. 주요 개념 상세 설명

#### 2.1. 버텍스 버퍼 (Vertex Buffer)

버텍스 버퍼는 GPU가 렌더링할 모델의 정점 데이터를 담는 메모리 공간입니다. 각 정점은 위치, 색상, 텍스처 좌표 등 다양한 속성을 가질 수 있습니다.

##### **1단계: 정점 데이터 구조 정의 (`struct Vertex`)**

먼저 C++ 코드에서 정점의 구조를 정의합니다. PDF에서는 위치(`pos`)와 색상(`color`)을 갖는 `Vertex` 구조체를 사용합니다. `glm` 라이브러리를 사용하여 벡터 형태(`vec2`, `vec3`)로 정의합니다.

```cpp
#include <glm/glm.hpp>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};
```

이 구조체를 기반으로 실제 정점 데이터를 배열로 정의합니다.

```cpp
const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};
```

##### **2단계: 정점 데이터 구조를 Vulkan에 설명하기**

Vulkan은 C++의 `struct`를 알지 못하므로, 이 데이터가 메모리에 어떻게 배치되어 있는지, 그리고 셰이더의 어떤 입력 변수와 연결되는지를 명시적으로 알려주어야 합니다.

1.  **`VkVertexInputBindingDescription`**: 정점 데이터 묶음(Binding)의 속성을 정의합니다.
    - `binding`: 바인딩 번호. 보통 0번부터 시작합니다.
    - `stride`: 한 정점에서 다음 정점까지의 메모리 간격(byte 단위). `sizeof(Vertex)`가 됩니다.
    - `inputRate`: 정점 데이터를 얼마나 자주 읽을지 지정합니다.
        - `VK_VERTEX_INPUT_RATE_VERTEX`: 정점마다 데이터를 읽습니다 (일반적인 경우).
        - `VK_VERTEX_INPUT_RATE_INSTANCE`: 인스턴스마다 데이터를 읽습니다 (인스턴스 렌더링 시).

    ```cpp
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    ```

2.  **`VkVertexInputAttributeDescription`**: 정점의 각 속성(Attribute)을 개별적으로 설명합니다.
    - `binding`: 이 속성이 속한 바인딩 번호 (위의 `bindingDescription`과 일치).
    - `location`: 셰이더 코드의 `layout(location = ...)`과 일치하는 번호.
    - `format`: 속성의 데이터 타입. 셰이더의 타입과 호환되어야 합니다.
        - `VK_FORMAT_R32G32_SFLOAT`: `vec2` (32비트 float 2개)
        - `VK_FORMAT_R32G32B32_SFLOAT`: `vec3` (32비트 float 3개)
    - `offset`: 정점 데이터 시작점으로부터 이 속성이 얼마나 떨어져 있는지(byte 단위). `offsetof` 매크로를 사용하면 편리합니다.

    ```cpp
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        // 위치(pos) 속성: location = 0
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        // 색상(color) 속성: location = 1
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
    ```

이 정보들은 그래픽스 파이프라인 생성 시 `VkPipelineVertexInputStateCreateInfo` 구조체에 담겨 전달됩니다.

##### **3단계: 버퍼 생성 및 메모리 할당**

정점 데이터를 GPU가 접근할 수 있는 메모리에 올려야 합니다. 이 과정은 크게 **버퍼 생성**과 **메모리 할당**으로 나뉩니다.

- **버퍼 (`VkBuffer`)**: 특정 용도와 크기를 가진 메모리 덩어리에 대한 핸들(이름표). 하지만 실제 메모리를 가지고 있지는 않습니다.
- **메모리 (`VkDeviceMemory`)**: GPU가 실제로 사용하는 메모리.

**최적의 성능을 위한 버퍼 복사 (Staging Buffer)**

CPU는 GPU 전용 메모리(`VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT`)에 직접 접근할 수 없는 경우가 많습니다. 그래서 보통 두 단계로 데이터를 전송합니다.

1.  **Staging Buffer 생성**: CPU가 쓰고 GPU가 읽을 수 있는 메모리(`VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT`)를 사용하여 임시 버퍼를 만듭니다.
2.  **데이터 복사**: CPU가 정점 데이터를 이 Staging Buffer에 `memcpy` 합니다.
3.  **Vertex Buffer 생성**: GPU만 접근할 수 있어 성능이 매우 빠른 메모리(`VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT`)를 사용하여 최종 버텍스 버퍼를 만듭니다.
4.  **버퍼 간 복사**: Vulkan 커맨드를 사용하여 Staging Buffer의 내용을 최종 Vertex Buffer로 복사합니다.
5.  **Staging Buffer 해제**: 복사가 끝나면 임시 Staging Buffer와 그 메모리를 해제합니다.

이 방식은 초기 설정이 복잡하지만, 렌더링 시 GPU가 가장 빠른 메모리에 접근하게 하여 성능을 극대화합니다.

**버퍼 생성 함수 `createBuffer`**

PDF에서는 이 과정을 캡슐화한 `createBuffer` 헬퍼 함수를 사용합니다.

```cpp
// 가상 코드
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    // 1. VkBufferCreateInfo로 버퍼 생성 정보 설정
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.size = size; // 버퍼 크기
    bufferInfo.usage = usage; // 버퍼 용도
    vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);

    // 2. vkGetBufferMemoryRequirements로 필요한 메모리 크기 및 타입 확인
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    // 3. VkMemoryAllocateInfo로 메모리 할당 정보 설정
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties); // 적합한 메모리 타입 찾기

    // 4. vkAllocateMemory로 실제 메모리 할당
    vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);

    // 5. vkBindBufferMemory로 버퍼와 메모리 연결
    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}
```

##### **4단계: 버텍스 버퍼 바인딩**

커맨드 버퍼 기록 시, `vkCmdBindVertexBuffers` 함수를 호출하여 생성된 버텍스 버퍼를 파이프라인에 연결합니다. 이제 `vkCmdDraw`가 호출되면 GPU는 이 버퍼에서 정점 데이터를 읽어 렌더링을 수행합니다.

```cpp
VkBuffer vertexBuffers[] = {vertexBuffer};
VkDeviceSize offsets[] = {0};
vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

// 정점 4개, 인스턴스 1개로 드로우 콜
vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
```

---

#### 2.2. 유니폼 버퍼 (Uniform Buffer) 와 디스크립터 (Descriptor)

유니폼 버퍼는 셰이더에서 사용되는 '전역 변수'와 같습니다. 모델의 변환(이동, 회전, 크기), 카메라의 위치, 조명 상태 등 렌더링 중인 모든 정점에 동일하게 적용되는 데이터를 담습니다.

##### **1단계: 유니폼 버퍼 객체(UBO) 구조 정의**

버텍스 버퍼와 마찬가지로 C++에서 UBO의 구조를 정의합니다. 이 예제에서는 모델, 뷰, 투영 행렬을 담습니다.

```cpp
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
```

##### **2단계: 디스크립터 - 버퍼와 셰이더의 연결고리**

GPU에게 "이 유니폼 버퍼를 사용해라"라고 알려주려면 **디스크립터(Descriptor)**라는 메커니즘이 필요합니다. 디스크립터는 셰이더가 GPU 메모리 상의 리소스(버퍼, 텍스처 등)에 접근할 수 있게 해주는 '포인터' 또는 '참조'입니다.

설정 과정은 3단계로 이루어집니다.

1.  **`VkDescriptorSetLayout` 생성**: "우리 파이프라인은 이런 종류의 디스크립터들을 사용할 거야"라고 **설계도**를 만드는 과정입니다.
    - `binding`: 셰이더의 `layout(binding = ...)`과 일치하는 번호.
    - `descriptorType`: 리소스의 종류. 여기서는 `VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER`.
    - `descriptorCount`: 이 바인딩에 몇 개의 디스크립터가 있는지 (배열이 아니라면 보통 1).
    - `stageFlags`: 이 디스크립터를 어느 셰이더 단계에서 사용할지 지정. 예: `VK_SHADER_STAGE_VERTEX_BIT`.

    ```cpp
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // 버텍스 셰이더에서 사용

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
    ```

2.  **`VkDescriptorPool` 생성**: 실제 디스크립터들을 할당할 **메모리 풀**을 만드는 과정입니다. 어떤 종류(`type`)의 디스크립터를 몇 개(`descriptorCount`)나 담을 수 있는지 미리 지정합니다.

    ```cpp
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); // 프레임마다 하나씩

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
    ```

3.  **`VkDescriptorSet` 할당**: 위에서 만든 설계도(`descriptorSetLayout`)와 메모리 풀(`descriptorPool`)을 사용하여 실제 **디스크립터 셋**을 할당받습니다.

    ```cpp
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout; // 이 레이아웃에 맞춰 할당

    vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
    ```

##### **3단계: 유니폼 버퍼 생성 및 디스크립터 업데이트**

- **버퍼 생성**: 버텍스 버퍼와 동일한 방식으로 `createBuffer`를 사용하여 유니폼 버퍼를 생성합니다. 단, CPU에서 매 프레임 데이터를 업데이트해야 하므로 GPU 전용 메모리가 아닌 CPU-GPU 공유 메모리(`VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT`)를 사용합니다. 또한, 여러 프레임이 동시에 처리될 때(in-flight frames) 데이터 충돌을 막기 위해 프레임 개수만큼 버퍼를 만듭니다.

- **디스크립터 업데이트**: `vkUpdateDescriptorSets` 함수를 호출하여 "이 디스크립터 셋의 0번 바인딩은 저 유니폼 버퍼를 가리킨다"고 **실제 연결 정보**를 업데이트합니다.

    ```cpp
    // 각 프레임에 대해 반복
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i]; // i번째 프레임의 유니폼 버퍼
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = descriptorSets[i]; // i번째 프레임의 디스크립터 셋
        descriptorWrite.dstBinding = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
    ```

##### **4단계: 셰이더 코드 수정 및 디스크립터 바인딩**

- **셰이der**: 버텍스 셰이더에서 `layout(binding = 0)`을 사용하여 UBO를 선언하고, `ubo.model`, `ubo.view`, `ubo.proj`를 사용하여 정점 위치를 변환합니다.

    ```glsl
    #version 450

    layout(binding = 0) uniform UniformBufferObject {
        mat4 model;
        mat4 view;
        mat4 proj;
    } ubo;

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec3 inColor;

    layout(location = 0) out vec3 fragColor;

    void main() {
        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
        fragColor = inColor;
    }
    ```

- **바인딩**: 커맨드 버퍼 기록 시, `vkCmdBindDescriptorSets`를 호출하여 준비된 디스크립터 셋을 파이프라인에 연결합니다.

    ```cpp
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
    ```

##### **5단계: 애니메이션 구현 (UBO 업데이트)**

매 프레임을 그리는 `drawFrame` 함수 안에서 `updateUniformBuffer` 함수를 호출합니다. 이 함수는 현재 시간에 따라 모델 변환 행렬(`ubo.model`)을 새로 계산하고, `memcpy`를 통해 현재 프레임의 유니폼 버퍼 내용을 업데이트합니다. 이로써 매 프레임 다른 변환이 적용되어 사각형이 회전하게 됩니다.

```cpp
void updateUniformBuffer(uint32_t currentImage) {
    // 시간 계산
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    // Z축을 기준으로 시간에 따라 회전하는 모델 행렬 생성
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // 카메라 설정 (View), 투영 설정 (Projection)
    ubo.view = glm::lookAt(...);
    ubo.proj = glm::perspective(...);

    // 계산된 UBO 데이터를 현재 프레임의 유니폼 버퍼로 복사
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
```

---

### 3. 시험 대비 핵심 요약

- **버퍼의 종류와 목적**
    - **버텍스 버퍼**: 정점별 고유 데이터(위치, 색상, UV) 저장. `VK_BUFFER_USAGE_VERTEX_BUFFER_BIT`.
    - **인덱스 버퍼**: 정점 재사용을 위한 인덱스 저장. `VK_BUFFER_USAGE_INDEX_BUFFER_BIT`.
    - **유니폼 버퍼**: 모든 정점에 동일하게 적용되는 전역 데이터(변환 행렬, 조명) 저장. `VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT`.
    - **Staging 버퍼**: CPU 데이터를 GPU 전용 메모리로 효율적으로 옮기기 위한 임시 버퍼. `VK_BUFFER_USAGE_TRANSFER_SRC_BIT`.

- **버퍼 생성 및 관리 핵심 함수**
    - `vkCreateBuffer`: 버퍼 핸들 생성.
    - `vkGetBufferMemoryRequirements`: 버퍼에 필요한 메모리 요구사항 확인.
    - `vkAllocateMemory`: 실제 GPU 메모리 할당.
    - `vkBindBufferMemory`: 버퍼 핸들과 할당된 메모리를 연결.
    - `vkMapMemory` / `vkUnmapMemory`: CPU에서 GPU 메모리에 접근하기 위해 매핑/해제.
    - `memcpy`: 매핑된 메모리에 데이터 복사.
    - `vkCmdCopyBuffer`: GPU 내에서 버퍼 간 데이터 복사.

- **디스크립터 관련 핵심 워크플로우**
    1.  **Layout 정의 (`vkCreateDescriptorSetLayout`)**: 셰이더가 사용할 리소스의 종류와 바인딩을 설계.
    2.  **Pool 생성 (`vkCreateDescriptorPool`)**: 디스크립터 셋을 할당할 메모리 풀을 생성.
    3.  **Set 할당 (`vkAllocateDescriptorSets`)**: 풀에서 실제 디스크립터 셋을 할당.
    4.  **Set 업데이트 (`vkUpdateDescriptorSets`)**: 디스크립터 셋이 실제 버퍼나 이미지를 가리키도록 정보를 업데이트.
    5.  **바인딩 (`vkCmdBindDescriptorSets`)**: 렌더링 커맨드 기록 시, 파이프라인에 디스크립터 셋을 연결.

- **GLSL 셰이더에서의 연결**
    - `layout(location = ...)`: C++의 `VkVertexInputAttributeDescription`과 연결.
    - `layout(binding = ...)`: C++의 `VkDescriptorSetLayoutBinding`과 연결.

이 문서를 통해 버퍼와 디스크립터의 개념을 확실히 이해하고, 시험에서 관련 문제가 나왔을 때 각 함수의 역할과 전체 흐름을 참고하여 문제를 해결할 수 있을 것입니다.
