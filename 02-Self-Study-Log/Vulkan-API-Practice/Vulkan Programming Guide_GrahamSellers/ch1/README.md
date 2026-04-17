# Instance, Device, Queue


vulkan instance는 애플리케이션의 단위

Device는 하나의 하드웨어나 연결된 하드웨어의 집합

physical device는 물리장치
(logical) device는 instance가 생성하며 device를 감싼다.
(logical) device는 특정 물리장치와 연결된 자원의 예약을 표현

하나의 물리 장치에서 다양한 (logical) device가 생성이 가능..


app - interface - physical D - Logical D - Queue

```cpp
void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // stype이 모든 구조체의 첫 멤버
        appInfo.pApplicationName = "Ray Traced Scene";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2; //사용하는 Vulkan 버전


        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size()); 
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());//InstanceLayer의 수
            //layer는 불칸 API 사이에서 로깅, 프로파일링, 디버깅, 추가적인 기능에 이용됨.
            createInfo.ppEnabledLayerNames = validationLayers.data();//instanceLayer 이름
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }

}
```

DeviceMemoryProperty의..

VkMemoryType
VK_MEMORY_PROPERTY_DEVICE_LOACL_BIT : 메모리가 지역적
_DEVICE_LOACL_BIT : 메모리가 지역적
_HOST_VISIBLE_BIT : 메모리가 바로 연결되어 주 시스템에 의해서 Read , Write
_HOST_COHERENT_BIT : 동시에 주 메모리, 장치에 의해서 접근되면 일관성 있다는 의미(이 비트를 설정해야 캐시가 씻기기 전에도 쓰기 결과를 볼 수 있음)
_HOST_CACHED_BIT : 주 시스템에 캐시되었다는 것, 일반적으로 더 빨라지지만, 장치 접근에서 살짝 높은 지연을 가진다
_LAZILY_ALLOCATED_BIT : 연결된 힙에서 즉시 공간을 소모할 필요가 X, 즉 메모리 객체가 자원에서 사용될때까지 물리 메모리 할당을 미룰 수 있음.

이러한 메모리 종류는 VkMemoryType 구조체의 heapIndex로 보고함


각 장치에는 Queue가 달리는데, 이 안에 들어있는 명령어들을 수행함

QueueFamilyProperties에서 큐의 특성을 정의하는데
queueFlags는 전체적인 큐의 능력을 설명

### VK_QUEUE_~~
-_GRAPHICS_BIT : 점 선 삼각형을 그리는 그래픽연산
-_COMPUTE_BIT : 계산 셰이더를 배치하는 등의 계산 연산
-_TRANSFER_BIT : 버퍼와 이미지 내용을 복사하는 등의 전송 연산
-_SPARSE_BINDING_BIT : 듬성한 자원을 갱신하는데 사용되는 메모리 결속 연산

을 지원



