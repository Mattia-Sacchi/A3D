#version 330 core

in vec2 fs_UV;
in vec2 fs_LineCaps[2];
in vec4 fs_Color;
in float fs_FeatherSize;

out vec4 fragColor;

void main()
{
	float fAlpha = 1.0;

	if(fs_UV.x < fs_LineCaps[0].x) {
		float distanceX = ((fs_UV.x - fs_LineCaps[0].x) / fs_LineCaps[0].x);
		float distanceY = (fs_UV.y - fs_LineCaps[0].y) * 2;


		float realDistance = sqrt((distanceX * distanceX) + (distanceY * distanceY));

		if(realDistance > 1.0)
			discard;

		if(realDistance < (1.0 - fs_FeatherSize))
			fAlpha = 1.0;
		else
			fAlpha = 1.0 - smoothstep(1.0 - fs_FeatherSize, 1.0, realDistance);
	}
	else if(fs_UV.x > fs_LineCaps[1].x) {
		float distanceX = ((fs_UV.x - fs_LineCaps[1].x) / fs_LineCaps[0].x);
		float distanceY = (fs_UV.y - fs_LineCaps[1].y) * 2;


		float realDistance = sqrt((distanceX * distanceX) + (distanceY * distanceY));

		if(realDistance > 1.0)
			discard;

		if(realDistance < (1.0 - fs_FeatherSize))
			fAlpha = 1.0;
		else
			fAlpha = 1.0 - smoothstep(1.0 - fs_FeatherSize, 1.0, realDistance);
	}

	if(fAlpha <= 0)
		discard;

	fragColor = fs_Color * vec4(1.0, 1.0, 1.0, fAlpha);
}
