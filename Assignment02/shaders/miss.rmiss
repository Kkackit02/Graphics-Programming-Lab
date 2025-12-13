#version 460
#extension GL_EXT_ray_tracing : require

struct RayPayload {
    vec3 hitValue;
    bool isOccluded;
    bool isShadowRay;
};

layout(location = 0) rayPayloadInEXT RayPayload prd;

void main()
{
    vec3 backColor = vec3(0.02, 0.02, 0.02); 

    // vec3 backColor = vec3(0.5, 0.7, 1.0); 
    // vec3 backColor = vec3(1.0, 1.0, 1.0);
    // vec3 backColor = vec3(0.0, 0.0, 0.0); 

    prd.hitValue = backColor;
}
