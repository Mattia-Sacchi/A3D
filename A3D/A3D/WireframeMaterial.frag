#version 330 core

out vec4 fragColor;
in vec2 TexCoord;

void main()
{
    float edgeWidth = 0.03;

    // Distance from the edge (0 or 1 on UV edges)
    float distX = min(TexCoord.x, 1.0 - TexCoord.x);
    float distY = min(TexCoord.y, 1.0 - TexCoord.y);

    float edgeFactor = min(distX, distY);

    // Fade near the edge
    float alpha = smoothstep(0.0, edgeWidth, edgeFactor);

    fragColor = vec4(1.0, 1.0, 1.0, 1.0 - alpha);
}
