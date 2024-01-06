#version 330 core

in vec3 inVxPos;
out vec4 fragColor;

uniform sampler2D TextureSlot0;

void main() {
	// fragColor = vec4(inVxPos, 0.3f);
	fragColor = texture(TextureSlot0, vec2(inVxPos.x - 0.5, 0.5 - inVxPos.y));
}
