#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model[2];
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    int objectIndex = (gl_VertexIndex < 4) ? 0 : 1; // 8개의 정점 중 앞 4개는 0번 행렬, 뒤 4개는 1번 행렬 사용
    gl_Position = ubo.proj * ubo.view * ubo.model[objectIndex] * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
