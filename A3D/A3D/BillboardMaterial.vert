#version 330 core

layout (location = 0) in vec3 inVertex;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inNormal;

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

out vec3 WorldPos;
out vec2 TexCoord;
out vec3 Normal;

void main() {
    vec3 right = normalize(vec3(vMatrix[0][0], vMatrix[1][0], vMatrix[2][0]));
    vec3 up = normalize(vec3(vMatrix[0][1], vMatrix[1][1], vMatrix[2][1]));

    vec3 scale = vec3(
        length(vec3(mMatrix[0])),
        length(vec3(mMatrix[1])),
        length(vec3(mMatrix[2]))
    );

    right *= scale.x;
    up *= scale.y;

    vec3 center = vec3(mMatrix[3]);
    WorldPos = center + inVertex.x * right + inVertex.y * up;

    TexCoord = vec2(inTexCoord.x, 1- inTexCoord.y);
    Normal = mat3(mNormalMatrix) * inNormal;

    gl_Position = pMatrix * vMatrix * vec4(WorldPos, 1.0);
}
