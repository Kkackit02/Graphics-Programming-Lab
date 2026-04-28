#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_gpu_shader_int64 : require

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
struct ObjDesc {
    uint64_t vertexAddress;
    uint64_t indexAddress;
};
layout(set = 0, binding = 4, scalar) buffer perInstanceData { 
    ObjDesc i[]; 
} objDesc;


layout(location = 0) rayPayloadInEXT vec3 hitValue;
layout(location = 1) rayPayloadEXT bool isShadowed;

hitAttributeEXT vec2 attribs;

// Buffer reference bindings
layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices { uint i[]; };


vec3 getShadingNormal() {
    // Get the object descriptor for the instance
    ObjDesc desc = objDesc.i[gl_InstanceCustomIndexEXT];

    // Get the buffer references
    Vertices vertices = Vertices(desc.vertexAddress);
    Indices  indices  = Indices(desc.indexAddress);

    // Get the 3 indices of the triangle
    uint i0 = indices.i[3 * gl_PrimitiveID + 0];
    uint i1 = indices.i[3 * gl_PrimitiveID + 1];
    uint i2 = indices.i[3 * gl_PrimitiveID + 2];

    // Get the 3 vertices of the triangle
    vec3 v0 = vertices.v[i0].pos;
    vec3 v1 = vertices.v[i1].pos;
    vec3 v2 = vertices.v[i2].pos;

    // Get the 3 normals of the triangle
    vec3 n0 = vertices.v[i0].normal;
    vec3 n1 = vertices.v[i1].normal;
    vec3 n2 = vertices.v[i2].normal;

    // Interpolate normal
    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    vec3 normal = n0 * barycentrics.x + n1 * barycentrics.y + n2 * barycentrics.z;

    // return normalize((gl_ObjectToWorldEXT * vec4(normal, 0.0)).xyz);
    return normalize(mat3(gl_ObjectToWorldEXT) * normal);
}

void main() {
    vec3 worldPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    uint idx = gl_InstanceCustomIndexEXT; 
    vec3 objectColor = vec3(colors[idx].r, colors[idx].g, colors[idx].b);

    vec3 normal = getShadingNormal();
    
    // Ambient light
    vec3 ambient = 0.1 * objectColor;
    vec3 result = ambient;

    // Loop through lights
    for (int i = 0; i < ubo.lightCount; ++i) {
        if (ubo.lights[i].enabled == 1) {
            vec3 lightDir = ubo.lights[i].position - worldPos;
            float lightDist = length(lightDir);
            lightDir = normalize(lightDir);

            // Cast shadow ray
            isShadowed = true; // Assume shadowed
            float tmin = 0.001;
            float tmax = lightDist;
            traceRayEXT(topLevelAS, 
                        gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT, 
                        0xFF, 
                        0, // sbt offset
                        0, // sbt stride
                        1, // miss shader index (for shadow)
                        worldPos + normal * tmin, // origin
                        tmin, 
                        lightDir, 
                        tmax, 
                        1); // payload location

            if (!isShadowed) {
                // Attenuation factor (inverse square law)
                float attenuation = 1.0 / max(1.0, lightDist * lightDist);

                // Diffuse
                float diff = max(dot(normal, lightDir), 0.0);
                vec3 diffuse = diff * ubo.lights[i].color * ubo.lights[i].intensity * attenuation;

                // Specular
                vec3 viewDir = normalize(ubo.cameraPos - worldPos);
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
                vec3 specular = spec * ubo.lights[i].color * ubo.lights[i].intensity * attenuation;

                result += (diffuse + specular) * objectColor;
            }
        }
    }

    hitValue = result;
}
