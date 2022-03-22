#version 450

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout( push_constant ) uniform constants {
    float x;
    float y;
} pConsts;

void main() {
    gl_Position = ubo.model * ubo.view * ubo.proj * vec4(position.x + pConsts.x, position.y + pConsts.y, position.z, 1.0);
    fragColor = vec3(0.0,0.2,1.0);
}
