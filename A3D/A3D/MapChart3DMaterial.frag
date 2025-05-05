#version 330 core

in vec3 BarycentricPos;
in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;
out vec4 fragColor;

// Alpha = input value
vec4 colorLowLow = vec4(0.09, 0.60, 0.70, 0.00);
vec4 colorMidLow = vec4(0.04, 0.44, 0.16, 0.25);
vec4 colorMidMid = vec4(1.00, 0.87, 0.00, 0.50);
vec4 colorMidHig = vec4(1.00, 0.50, 0.13, 0.75);
vec4 colorHigHig = vec4(1.00, 0.18, 0.13, 1.00);

vec3 interpolateColor(float value) {
	if(value < colorMidLow.a)
		return mix(colorLowLow.rgb, colorMidLow.rgb, (value - colorLowLow.a) / (colorMidLow.a - colorLowLow.a));
	else if(value < colorMidMid.a)
		return mix(colorMidLow.rgb, colorMidMid.rgb, (value - colorMidLow.a) / (colorMidMid.a - colorMidLow.a));
	else if(value < colorMidHig.a)
		return mix(colorMidMid.rgb, colorMidHig.rgb, (value - colorMidMid.a) / (colorMidHig.a - colorMidMid.a));
	else
		return mix(colorMidHig.rgb, colorHigHig.rgb, (value - colorMidHig.a) / (colorHigHig.a - colorMidHig.a));
}

void main() {
	vec2 InputCoordinate = TexCoord;

	float stepN = 0.98;
	float intensityX = step(stepN, mod(InputCoordinate.x, 1.0)) * 1.0;
	float intensityZ = step(stepN, mod(InputCoordinate.y, 1.0)) * 1.0;

	vec3 gridColor = vec3(intensityX * 0.5, 0.0, intensityZ * 0.5);

	vec3 color = mix(interpolateColor(LocalPos.y), gridColor, min(intensityX + intensityZ, 1.0));

	fragColor = vec4(color, 1.0);
}
