#version 330 core

in vec3 WorldPos;
out vec4 fragColor;

uniform samplerCube EnvironmentMapTexture;

const float PI = 3.14159265359;

void main()
{

	vec3 N = normalize(WorldPos);

	vec3 irradiance = vec3(0.0);

	vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), N));
	vec3 up = normalize(cross(N, right));

	float sampleDelta = 0.025;
	float nrSamples = 0.0;

	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {

		float sinPhi = sin(phi);
		float cosPhi = cos(phi);

		for(float theta = 0; theta < 0.5 * PI; theta += sampleDelta) {
			nrSamples += 1.0;
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);

			if(cosTheta * sinTheta < 0.01)
				continue;

			vec3 tangentSample = vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += texture(EnvironmentMapTexture, sampleVec).rgb * cosTheta * sinTheta;
		}

	}

	irradiance = PI * irradiance * (1.0 / nrSamples);
	fragColor = vec4(irradiance, 1.0);
}
