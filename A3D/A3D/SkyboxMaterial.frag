#version 330 core

in vec3 WorldPos;
out vec4 fragColor;

uniform samplerCube EnvironmentMapTexture;

void main()
{
	vec4 envColorRGBA = texture(EnvironmentMapTexture, WorldPos);
	vec3 envColor = envColorRGBA.rgb;

	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0 / 2.2));

	fragColor = vec4(envColor, envColorRGBA.a);
}
