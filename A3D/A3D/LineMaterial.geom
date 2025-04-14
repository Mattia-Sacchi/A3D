#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

layout (std140) uniform MeshUBO_Data {
	mat4 pMatrix;
	mat4 vMatrix;
	mat4 mMatrix;

	mat4 mvMatrix;
	mat4 mvpMatrix;

	mat4 mNormalMatrix;
	mat4 mvNormalMatrix;
	mat4 mvpNormalMatrix;
};

layout (std140) uniform LineUBO_Data {
	float LineThickness;
	float FeatherSize;
};

in vec4 vs_Color[];

out vec2 fs_UV;
out vec2 fs_LineCaps[2];
out vec4 fs_Color;
out float fs_FeatherSize;

void main()
{
	vec3 p0 = (mvMatrix * gl_in[0].gl_Position).xyz;
	vec3 p1 = (mvMatrix * gl_in[1].gl_Position).xyz;

	vec3 dir = normalize(p1 - p0);
	vec3 viewDir = vec3(0.0, 0.0, -1.0);
	vec3 side = normalize(cross(viewDir, dir)) * (LineThickness * 0.5);

	vec3 capExtraLength = normalize(p1 - p0) * (LineThickness * 0.5);
	float capUvX = (LineThickness * 0.5) / length((p1 + capExtraLength) - (p0 - capExtraLength));

	vec3 corners[4];
	corners[0] = p0 - side - capExtraLength;
	corners[1] = p0 + side - capExtraLength;
	corners[2] = p1 - side + capExtraLength;
	corners[3] = p1 + side + capExtraLength;

	vec2 uvs[4];
	uvs[0] = vec2(0.0, 0.0);
	uvs[1] = vec2(0.0, 1.0);
	uvs[2] = vec2(1.0, 0.0);
	uvs[3] = vec2(1.0, 1.0);

	vec4 colors[4];
	colors[0] = vs_Color[0];
	colors[1] = vs_Color[0];
	colors[2] = vs_Color[1];
	colors[3] = vs_Color[1];

	for(int i = 0; i < 4; ++i) {
		fs_LineCaps[0] = vec2(capUvX, 0.5);
		fs_LineCaps[1] = vec2(1.0 - capUvX, 0.5);

		fs_UV = uvs[i];
		fs_Color = colors[i];
		fs_FeatherSize = FeatherSize;
		gl_Position = pMatrix * vec4(corners[i], 1.0);
		EmitVertex();
	}

	EndPrimitive();
}
