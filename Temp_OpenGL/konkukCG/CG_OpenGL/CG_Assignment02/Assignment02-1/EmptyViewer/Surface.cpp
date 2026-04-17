#include "Surface.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

using namespace std;

glm::vec3 Surface::shade(const Ray& ray,
    const glm::vec3& hit_point,
    const glm::vec3& normal,
    const Light& light, 
    const std::vector<Surface*>& surfaces) const
{
    // 뷰 벡터: 카메라 방향
    glm::vec3 V = glm::normalize(-ray.direction);

    // 광원 벡터: 히트 지점에서 광원까지
    glm::vec3 L = glm::normalize(light.position - hit_point);


    // 광선이 다른 오브젝트에 닿으면 그림자 상태
    Ray shadowRay(hit_point + 1e-4f * normal, L); // 섬세하게 오프셋 줘서 표면에서 바로 시작하지 않도록

    for (const auto& obj : surfaces)
    {
        float t_shadow;
        glm::vec3 shadow_hit, shadow_normal;

        // 이 광선이 어떤 오브젝트와 교차한다면,
        if (obj->intersect(shadowRay, t_shadow, shadow_hit, shadow_normal))
        {
            // 광원과의 거리보다 짧게 막히면 그림자임
            if (t_shadow > 0.001f && t_shadow < glm::length(light.position - hit_point))
            {
                return material.ka * light.intensity; // ambient만 반환
            }
        }
    }

    // 반사 벡터 (Phong reflection)
    glm::vec3 R = glm::reflect(-L, normal);

    // Ambient
    glm::vec3 ambient = material.ka * light.intensity;

    // Diffuse
    float diff = std::max(glm::dot(normal, L), 0.0f);
    glm::vec3 diffuse = material.kd * diff * light.intensity;

    // Specular
    float spec = std::pow(std::max(glm::dot(R, V), 0.0f), material.specularPower);
    glm::vec3 specular = material.ks * spec * light.intensity;

    glm::vec3 color = ambient + diffuse + specular;

    return glm::clamp(color, 0.0f, 1.0f); // [0,1] 범위로 클램핑
}
