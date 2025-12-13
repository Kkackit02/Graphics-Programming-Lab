#version 460
#extension GL_EXT_ray_tracing : require

struct RayPayload {
    vec3 hitValue;
    bool isOccluded;
    bool isShadowRay;
};

layout(location = 0) rayPayloadInEXT RayPayload prd;

void main() {
    prd.isOccluded = false;
}