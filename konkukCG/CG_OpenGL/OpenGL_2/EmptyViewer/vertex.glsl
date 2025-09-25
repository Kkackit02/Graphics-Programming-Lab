// vertex.glsl
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

uniform mat4 modeling;
uniform mat4 camera;
uniform mat4 projection;
uniform mat4 modeling_inv_tr;

out vec4 wPosition;
out vec4 wNormal;

void main() {
    gl_Position = projection * camera * modeling * position;
    wPosition = modeling * position;
    wNormal = modeling_inv_tr * normal;
}
