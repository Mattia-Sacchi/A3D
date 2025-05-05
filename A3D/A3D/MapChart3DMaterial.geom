#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 fs_WorldPos[3];
in vec2 fs_TexCoord[3];
in vec3 fs_Normal[3];
in vec3 fs_LocalPos[3];

out vec3 WorldPos;
out vec2 TexCoord;
out vec3 Normal;
out vec3 LocalPos;

out vec3 BarycentricPos;

void main() {
    vec3 barycentricPos[3];
	barycentricPos[0] = vec3(1.0, 0.0, 0.0);
	barycentricPos[1] = vec3(0.0, 1.0, 0.0);
	barycentricPos[2] = vec3(0.0, 0.0, 1.0);

    for (int i = 0; i < 3; ++i) {
        BarycentricPos = barycentricPos[i];
		WorldPos = fs_WorldPos[i];
		TexCoord = fs_TexCoord[i];
		Normal = fs_Normal[i]:
		LocalPos = fs_LocalPos[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}
