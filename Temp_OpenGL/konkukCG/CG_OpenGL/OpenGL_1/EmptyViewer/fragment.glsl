in vec4 position;
in vec4 normal;
uniform mat4 modeling;
uniform mat4 modeling_inv_tr;
uniform mat4 camera;
uniform mat4 projection;
out vec4 color;
vec4 shade(vec4 wp, vec4 wn) { // Shading code goes here. }
void main()
{
gl_Position = projection * camera * modeling * position;
vec4 wPos = modeling * position;
vec4 wNormal = modeling_inv_tr * normal;
color = shade(wPos, wNormal);
}