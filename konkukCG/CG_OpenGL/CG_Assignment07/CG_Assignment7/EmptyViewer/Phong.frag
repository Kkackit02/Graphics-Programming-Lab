#version 330 core

in vec4 wPosition;
in vec4 wNormal;
out vec4 outColor;

// uniform으로 전달받는 값들
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 Ia;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;

vec3 gammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

vec4 shade(vec4 wp, vec4 wn) {
    vec3 N = normalize(wn.xyz);
    vec3 L = normalize(lightPos - wp.xyz);
    vec3 V = normalize(vec3(0, 0, 3) - wp.xyz); // eye pos 고정
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), shininess);

    vec3 ambient  = ka * Ia;
    vec3 diffuse  = kd * lightColor * diff;
    vec3 specular = ks * lightColor * spec;

    vec3 linearColor = ambient + diffuse + specular;
    vec3 finalColor = gammaCorrect(linearColor);

    return vec4(finalColor, 1.0);
}

void main() {
    outColor = shade(wPosition, wNormal);
}
