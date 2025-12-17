# Vulkan 학습 자료집: 03. 텍스처 및 기타 리소스

이 문서는 `수정_03Vulkan (2).pdf` 파일의 내용을 바탕으로, 3D 그래픽스의 핵심 요소인 **텍스처 매핑(Texture Mapping)**과 **뎁스 버퍼링(Depth Buffering)**에 대해 상세히 설명합니다. 시험에서 관련 개념을 빠르고 정확하게 찾아볼 수 있도록 구성되었습니다.

---

### 1. 학습 목표

- **텍스처 매핑**: 3D 객체 표면에 2D 이미지를 입히는 전체 과정을 이해합니다.
    - 이미지 파일 로딩, `VkImage`, `VkImageView`, `VkSampler`의 개념과 생성 방법을 학습합니다.
    - Staging Buffer를 사용해 이미지를 GPU 메모리로 효율적으로 전송하는 방법을 익힙니다.
    - 이미지 레이아웃(Image Layout)과 레이아웃 전환(Layout Transition)의 중요성을 이해합니다.
- **뎁스 버퍼링**: 3D 씬에서 객체의 앞뒤 관계를 올바르게 처리하여 깨진 그래픽(Z-fighting)을 방지하는 방법을 배웁니다.
    - 뎁스 버퍼(Depth Buffer)의 원리를 이해하고, 뎁스 이미지를 생성 및 설정하는 방법을 학습합니다.
- **실습**: `stb_image` 라이브러리로 이미지를 로드하여 회전하는 3D 사각형에 텍스처를 입히고, 뎁스 버퍼링을 적용합니다.

---

### 2. 주요 개념 상세 설명: 텍스처 매핑 (Texture Mapping)

텍스처 매핑은 밋밋한 3D 모델에 사실적인 표면을 입히는 과정입니다. 이를 위해 Vulkan에서는 여러 객체를 유기적으로 조합해야 합니다.

#### **텍스처 매핑 전체 워크플로우**

1.  **이미지 로드**: `stb_image` 같은 라이브러리를 사용해 이미지 파일(jpg, png 등)을 CPU 메모리로 로드.
2.  **Staging Buffer 생성**: 로드한 픽셀 데이터를 GPU로 옮기기 위한 임시 버퍼(CPU에서 접근 가능)를 생성하고 데이터 복사.
3.  **`VkImage` 생성**: 텍스처를 저장할 GPU 전용 이미지 객체(`VkImage`)를 생성.
4.  **레이아웃 전환 (1)**: `VkImage`의 레이아웃을 '복사 대상'(`VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`)으로 변경.
5.  **버퍼에서 이미지로 복사**: Staging Buffer의 데이터를 `VkImage`로 복사.
6.  **레이아웃 전환 (2)**: `VkImage`의 레이아웃을 '셰이더 읽기 전용'(`VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`)으로 변경.
7.  **`VkImageView` 생성**: `VkImage`의 특정 부분을 어떻게 바라볼지 정의하는 이미지 뷰 생성.
8.  **`VkSampler` 생성**: 셰이더가 텍스처를 샘플링(색상 추출)할 방법을 정의하는 샘플러 생성 (필터링, 주소 지정 방식 등).
9.  **디스크립터 업데이트**: 생성된 이미지 뷰와 샘플러를 디스크립터 셋에 연결하여 셰이더에서 사용할 수 있도록 함.

---

#### **단계별 상세 설명**

##### **1단계: 정점 데이터에 텍스처 좌표(TexCoord) 추가**

셰이더에게 정점의 어느 위치에 텍스처의 어느 부분을 매핑할지 알려주기 위해 `Vertex` 구조체에 `texCoord` (`vec2`)를 추가합니다. 텍스처 좌표는 보통 (0,0)에서 (1,1) 사이의 값을 가집니다.

```cpp
struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord; // 텍스처 좌표 추가
};

// 정점 데이터에 텍스처 좌표 값 추가
const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 우측 하단
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 좌측 하단
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},   // 좌측 상단
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}  // 우측 상단
};

// Vertex Input Attribute Description에 texCoord 추가
attributeDescriptions[2].location = 2;
attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
```

##### **2단계: 이미지 로드 및 Staging Buffer 생성**

`stb_image.h` 라이브러리를 포함하고, `stbi_load` 함수로 이미지 파일을 불러옵니다.

```cpp
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int texWidth, texHeight, texChannels;
// STBI_rgb_alpha: RGB 채널만 있어도 알파 채널(A)을 추가하여 4바이트로 정렬
stbi_uc* pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
VkDeviceSize imageSize = texWidth * texHeight * 4;

// Staging Buffer 생성 및 데이터 복사
VkBuffer stagingBuffer;
VkDeviceMemory stagingBufferMemory;
createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

void* data;
vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
memcpy(data, pixels, static_cast<size_t>(imageSize));
vkUnmapMemory(device, stagingBufferMemory);

stbi_image_free(pixels); // 원본 픽셀 데이터는 해제
```

##### **3단계: `VkImage`와 `VkImageView` 생성**

텍스처를 담을 `VkImage`와 이를 바라볼 `VkImageView`를 생성합니다.

- **`VkImageCreateInfo`**: 이미지 생성 정보
    - `imageType`: `VK_IMAGE_TYPE_2D` (2D 텍스처)
    - `extent`: 이미지의 너비, 높이, 깊이.
    - `format`: 픽셀 형식. `stbi_load`에서 `STBI_rgb_alpha`를 썼으므로 `VK_FORMAT_R8G8B8A8_SRGB`가 적합.
    - `tiling`: `VK_IMAGE_TILING_OPTIMAL` (GPU에 최적화된 배치).
    - `usage`: 이미지의 용도.
        - `VK_IMAGE_USAGE_TRANSFER_DST_BIT`: 다른 버퍼로부터 데이터를 복사받을 수 있음.
        - `VK_IMAGE_USAGE_SAMPLED_BIT`: 셰이더에서 샘플링(읽기)할 수 있음.
- **`VkImageViewCreateInfo`**: 이미지 뷰 생성 정보
    - `image`: 대상 `VkImage`.
    - `viewType`: `VK_IMAGE_VIEW_TYPE_2D`.
    - `format`: `VkImage`와 동일한 형식.
    - `subresourceRange`: 이미지의 어느 부분을 뷰로 만들지 지정.

##### **4단계: 이미지 레이아웃 전환과 데이터 복사**

GPU는 작업을 최적화하기 위해 이미지의 메모리 레이아웃을 변경합니다. 따라서 작업에 맞는 레이아웃으로 직접 전환해주어야 합니다. 이 과정은 **파이프라인 배리어(Pipeline Barrier)**를 사용합니다.

```cpp
// 헬퍼 함수: transitionImageLayout
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.oldLayout = oldLayout; // 이전 레이아웃
    barrier.newLayout = newLayout; // 목표 레이아웃
    barrier.image = image;
    // ... (subresourceRange 등 설정)

    // 어떤 파이프라인 단계 사이에 배리어를 둘지 결정
    // 예: TRANSFER 단계 이후에 FRAGMENT_SHADER 단계가 이 리소스에 접근해야 함
    sourceStage = ...;
    destinationStage = ...;
    barrier.srcAccessMask = ...; // 이전 작업의 접근 유형
    barrier.dstAccessMask = ...; // 다음 작업의 접근 유형

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

// 실제 사용
// 1. 복사 대상으로 레이아웃 변경
transitionImageLayout(textureImage, ..., VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
// 2. Staging Buffer에서 VkImage로 복사
copyBufferToImage(stagingBuffer, textureImage, ...);
// 3. 셰이더 읽기용으로 레이아웃 변경
transitionImageLayout(textureImage, ..., VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
```

##### **5단계: `VkSampler` 생성**

샘플러는 셰이더가 텍스처 좌표로 텍스처를 읽을 때 어떻게 동작할지 정의합니다.

- **`VkSamplerCreateInfo`**:
    - `magFilter`, `minFilter`: 텍스처를 확대/축소할 때 사용할 필터링 (`VK_FILTER_LINEAR`는 선형 보간).
    - `addressModeU/V/W`: 텍스처 좌표가 [0, 1] 범위를 벗어났을 때 처리 방식.
        - `VK_SAMPLER_ADDRESS_MODE_REPEAT`: 텍스처를 반복.
        - `VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE`: 경계의 색상 값을 사용.
    - `anisotropyEnable`: 비등방성 필터링 사용 여부 (더 높은 품질의 텍스처 렌더링).

##### **6단계: 디스크립터 및 셰이더 수정**

- **디스크립터**: `VkDescriptorSetLayout`에 `VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER` 타입의 바인딩을 추가합니다. 이는 이미지 뷰와 샘플러를 하나로 묶어 셰이더에 전달합니다.
- **버텍스 셰이더**: 정점의 텍스처 좌표(`inTexCoord`)를 프래그먼트 셰이더로 넘겨줍니다.
    ```glsl
    layout(location = 2) in vec2 inTexCoord;
    layout(location = 1) out vec2 fragTexCoord; // 프래그먼트 셰이더로 전달

    void main() {
        // ...
        fragTexCoord = inTexCoord;
    }
    ```
- **프래그먼트 셰이더**: `sampler2D` 유니폼을 선언하고, `texture()` 함수를 사용해 넘어온 텍스처 좌표로 색상을 샘플링합니다.
    ```glsl
    layout(binding = 1) uniform sampler2D texSampler;
    layout(location = 1) in vec2 fragTexCoord;
    layout(location = 0) out vec4 outColor;

    void main() {
        outColor = texture(texSampler, fragTexCoord);
    }
    ```

---

### 3. 주요 개념 상세 설명: 뎁스 버퍼링 (Depth Buffering)

뎁스 버퍼링(또는 Z-버퍼링)은 3D 씬에서 어떤 픽셀이 카메라에 더 가까이 있는지 판단하여 올바른 픽셀만 그리도록 하는 기술입니다.

#### **뎁스 버퍼링 워크플로우**

1.  **뎁스 이미지 생성**: 깊이 값을 저장할 전용 `VkImage`와 `VkImageView`를 생성합니다.
    - **포맷**: `VK_FORMAT_D32_SFLOAT`, `VK_FORMAT_D24_UNORM_S8_UINT` 등 깊이 전용 포맷을 사용.
    - **용도**: `VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT`.
2.  **렌더 패스 수정**: `VkRenderPass`에 뎁스 버퍼를 위한 `VkAttachmentDescription`을 추가합니다.
    - `loadOp`: `VK_ATTACHMENT_LOAD_OP_CLEAR` (매 프레임 시작 시 깊이 버퍼를 1.0(가장 먼 값)으로 초기화).
    - `storeOp`: `VK_ATTACHMENT_STORE_OP_DONT_CARE` (렌더링이 끝나면 깊이 값은 필요 없으므로 저장하지 않음).
    - `finalLayout`: `VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL`.
3.  **서브패스 수정**: `VkSubpassDescription`에 `pDepthStencilAttachment`를 설정하여 뎁스 어태치먼트를 서브패스에 연결합니다.
4.  **프레임버퍼 수정**: `VkFramebuffer` 생성 시, 컬러 이미지 뷰와 함께 뎁스 이미지 뷰를 어태치먼트로 추가합니다.
5.  **그래픽스 파이프라인 수정**: `VkPipelineDepthStencilStateCreateInfo` 구조체를 설정하여 깊이 테스트를 활성화합니다.
    - `depthTestEnable`: `VK_TRUE`.
    - `depthWriteEnable`: `VK_TRUE`.
    - `depthCompareOp`: `VK_COMPARE_OP_LESS` (새로 그릴 픽셀의 깊이 값이 기존 값보다 작을 때(더 가까울 때)만 픽셀을 그림).

---

### 4. 시험 대비 핵심 요약

- **텍스처 매핑 순서**: `stbi_load` -> Staging Buffer -> `VkImage` -> Layout Transition (Copy Dst) -> `copyBufferToImage` -> Layout Transition (Shader Read) -> `VkImageView` -> `VkSampler` -> Descriptor Update.
- **이미지 레이아웃 전환**: `vkCmdPipelineBarrier`와 `VkImageMemoryBarrier`를 사용하며, `oldLayout`, `newLayout`, `srcAccessMask`, `dstAccessMask`가 핵심 파라미터.
    - **복사 전**: `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`
    - **셰이더 읽기 전**: `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`
- **핵심 텍스처 객체**:
    - `VkImage`: 실제 텍셀 데이터가 저장되는 GPU 객체.
    - `VkImageView`: `VkImage`를 어떻게 해석할지에 대한 뷰.
    - `VkSampler`: 텍스처를 어떻게 읽을지에 대한 규칙(필터링, 주소모드).
- **뎁스 버퍼링 설정**:
    1.  **뎁스용 `VkImage` / `VkImageView` 생성**: 깊이 포맷(`D32_SFLOAT` 등) 사용.
    2.  **`VkRenderPass`에 뎁스 어태치먼트 추가**: `loadOp`은 `CLEAR`, `storeOp`은 `DONT_CARE`.
    3.  **`VkFramebuffer`에 뎁스 뷰 추가**.
    4.  **파이프라인에서 `depthTestEnable` 활성화**.
- **셰이더에서의 텍스처 처리**:
    - **Vertex Shader**: `layout(location = ...)`으로 UV 좌표를 입력받아 `out` 변수로 Fragment Shader에 전달.
    - **Fragment Shader**: `layout(binding = ...)`으로 `sampler2D`를 받고, `texture()` 함수로 색상 추출.

이 문서는 텍스처와 뎁스 버퍼라는 두 가지 중요한 주제를 다룹니다. 각 단계에서 어떤 Vulkan 객체가 필요하고, 어떤 순서로 생성 및 설정되어야 하는지를 중심으로 학습하면 시험에 큰 도움이 될 것입니다.
