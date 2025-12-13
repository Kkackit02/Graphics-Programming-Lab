#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;

struct Light {
    vec3 position;
    float intensity;
    vec3 color;
    int enabled;
};
layout(binding = 2, set = 0) uniform UniformBufferObject {
    mat4 viewInverse;
    mat4 projInverse;
    vec3 cameraPos;
    float padding1;
    Light lights[3];
    int lightCount;
    float padding2[3];
} ubo;

struct Color4 { float r, g, b, a; };
layout(set = 0, binding = 3, std430) buffer InstanceColors {
    Color4 colors[];
};

struct Vertex {
  vec3 pos;
  vec3 normal;
};

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices { uint i[]; };
struct ObjDesc {
  uint64_t vertexAddress;
  uint64_t indexAddress;
};

layout(set = 0, binding = 4, scalar) buffer ObjDescBuffer {
  ObjDesc i[];
} objDesc;

struct RayPayload {
    vec3 hitValue;
    bool isOccluded;
    bool isShadowRay;
};

layout(location = 0) rayPayloadInEXT RayPayload prd;

hitAttributeEXT vec2 attribs;

vec3 getShadingNormal() {

    uint objId = gl_InstanceCustomIndexEXT;

    ObjDesc desc = objDesc.i[objId];
    Vertices vertices = Vertices(desc.vertexAddress);
    Indices indices = Indices(desc.indexAddress);

    uint ind0 = indices.i[3 * gl_PrimitiveID + 0];
    uint ind1 = indices.i[3 * gl_PrimitiveID + 1];
    uint ind2 = indices.i[3 * gl_PrimitiveID + 2];

    vec3 n0 = vertices.v[ind0].normal;
    vec3 n1 = vertices.v[ind1].normal;
    vec3 n2 = vertices.v[ind2].normal;

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

    vec3 normal = n0 * barycentrics.x + n1 * barycentrics.y + n2 * barycentrics.z;
    
    vec3 worldNormal = normalize(vec3(normal * gl_WorldToObjectEXT));

    if (dot(worldNormal, gl_WorldRayDirectionEXT) > 0.0) {
        worldNormal = -worldNormal;
    }


    return worldNormal;
}

void main() {
    // If this is a shadow ray, simply mark as occluded and return.
    // The payload.isOccluded was initialized to false before the trace.
    // If we hit, we set it to true.
    if (prd.isShadowRay) {
        prd.isOccluded = true;
        return;
    }

    vec3 worldPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    vec3 worldNormal = getShadingNormal();
    uint idx = gl_InstanceCustomIndexEXT; 
    vec3 objectColor = vec3(colors[idx].r, colors[idx].g, colors[idx].b);

    vec3 ambient = 0.1 * objectColor;
    vec3 result = ambient;

    vec3 viewDir = normalize(ubo.cameraPos - worldPos);

    for (int i = 0; i < ubo.lightCount; ++i) {
        if (ubo.lights[i].enabled == 0) continue;

        vec3 lightDir = normalize(ubo.lights[i].position - worldPos);
        float dist = length(ubo.lights[i].position - worldPos);

        // Shadow ray trace
        // Initialize payload for shadow ray
        prd.isOccluded = false; // Assume not occluded initially
        prd.isShadowRay = true; // Mark this ray as a shadow ray

        traceRayEXT(topLevelAS,
            gl_RayFlagsOpaqueEXT | gl_RayFlagsTerminateOnFirstHitEXT, // Terminate on first hit for shadow rays
            0xFF,
            0, // sbtOffset (Raygen uses 0, Miss uses 1, ShadowMiss uses 2. Hit is group 3. Shadow hit would be group 4, but we're not using a separate SH. Instead, we use the `isShadowRay` flag.)
            0, // sbtStride
            1, // missIndex, points to shadow.rmiss
            worldPos,
            0.001,
            lightDir,
            dist,
            0 // payload location for prd (RayPayload struct)
        );

        // Reset the flag for subsequent operations (though not strictly necessary as this prd is local to this trace)
        prd.isShadowRay = false;

        if (!prd.isOccluded) { // Only add diffuse/specular if not shadowed
            // Diffuse
            float diff = max(dot(worldNormal, lightDir), 0.0);
            vec3 diffuse = diff * ubo.lights[i].color;

            // Specular (Blinn-Phong)
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(worldNormal, halfwayDir), 0.0), 32.0);
            vec3 specular = spec * ubo.lights[i].color * 0.5;

            result += (diffuse + specular) * objectColor * ubo.lights[i].intensity;
        }
    }
    prd.hitValue = result;
}
