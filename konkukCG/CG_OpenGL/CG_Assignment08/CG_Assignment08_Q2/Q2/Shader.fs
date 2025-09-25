#version 330 core

out vec4 FragColor;

in vec3 wPosition;
in vec3 wNormal;
in vec3 wColor;
uniform vec3 eyePos;
uniform vec4 lightPos; 
uniform vec3 lightColor;
uniform vec3 Ia;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;

vec3 gammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}
vec4 Shading(vec3 wPosition, vec3 wNormal, vec3 wColor);

void main()
{
    FragColor = Shading(wPosition, wNormal, wColor);

}

vec4 Shading(vec3 wPosition, vec3 wNormal, vec3 wColor)
    {
    vec3 N = normalize(wNormal);
    vec3 L;
    if (lightPos.w == 0.0) 
    {
        // directional light
        L = normalize(-lightPos.xyz);  // 빛이 오는 방향이므로 반대 방향
    } 
    else {
        // point light
        L = normalize(lightPos.xyz - wPosition);
    }
    vec3 V = normalize(eyePos - wPosition);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), shininess);

    vec3 ambient  = ka * Ia;
    vec3 diffuse  = kd * lightColor * diff;
    vec3 specular = ks * lightColor * spec;

    vec3 linearColor = ambient + diffuse + specular;
    //vec3 finalColor = gammaCorrect(linearColor);

    linearColor = max(linearColor, ambient);// Apply color to the final color

    return vec4(linearColor, 1.0);

}
