#version 330 core

in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;
out vec4 fragColor;

void main() {
        vec3 color = mix(vec3(0.0, 0.8, 0.0), vec3(1.0, 0.0, 0.0), clamp(LocalPos.y, 0.0, 1.0));
        fragColor = vec4(color, 1.0);
}
