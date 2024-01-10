#version 330 core

in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Normal;
out vec4 fragColor;

layout (std140) uniform SceneUBO_Data {
	vec4 cameraPos;

	vec4 lightsPos[4];
	vec4 lightsColor[4];
};

uniform sampler2D AlbedoTexture;
uniform sampler2D NormalTexture;
uniform sampler2D MetallicTexture;
uniform sampler2D RoughnessTexture;
uniform sampler2D AOTexture;

uniform vec3 Color;

void main() {
	fragColor = vec4(Color, 1.f);
}
