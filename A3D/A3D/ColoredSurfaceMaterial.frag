#version 330 core

in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;
out vec4 fragColor;

void main() {
        fragColor = vec4((LocalPos.y + 0.15)* 0.9 ,LocalPos.y *LocalPos.z *0.25 * LocalPos.x,LocalPos.y * 0.1,1);
}
