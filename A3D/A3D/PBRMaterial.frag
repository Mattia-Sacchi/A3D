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

uniform samplerCube IrradianceTexture;
uniform samplerCube PrefilterTexture;
uniform sampler2D BrdfTexture;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(NormalTexture, TexCoord).xyz * 2.0 - 1.0;
	
	vec3 Q1 = dFdx(WorldPos);
	vec3 Q2 = dFdy(WorldPos);
	vec2 st1 = dFdx(TexCoord);
	vec2 st2 = dFdy(TexCoord);
	
	vec3 N = normalize(Normal);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	
	return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
	vec4 albedoRGBA = texture(AlbedoTexture, TexCoord);
	vec3 albedo = pow(albedoRGBA.rgb, vec3(2.2));
	float metallic = texture(MetallicTexture, TexCoord).r;
	float roughness = texture(RoughnessTexture, TexCoord).r;
	float ao = texture(AOTexture, TexCoord).r;
	
	vec3 N = getNormalFromMap();
	vec3 V = normalize(cameraPos.xyz - WorldPos.xyz);
	vec3 R = reflect(-V, N);
	
	vec3 F0 = mix(vec3(0.04), albedo, metallic);
	vec3 Lo = vec3(0.0);

	for(int i = 0; i < 4; ++i)
	{
		if(lightsPos[i].w <= -0.001f)
			continue;

		vec3 lightPos = lightsPos[i].xyz;
		vec3 lightColor = (lightsColor[i].rgb) * (1 + lightsColor[i].w + lightsPos[i].w);

		vec3 L = normalize(lightPos - WorldPos.xyz);
		vec3 H = normalize(V + L);
		float distance = length(lightPos - WorldPos.xyz);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColor * attenuation;
		
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
		
		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;
		
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	//vec3 F = fresnelSchlick(max(dot(N, V), 0.0), F0);

	vec3 kD = (1.0 - F) * (1.0 - metallic);
	vec3 irradiance = texture(IrradianceTexture, N).rgb;
	vec3 diffuse = irradiance * albedo;

	//vec3 specular = vec3(0.0);

	// if(F.x > 0.01 || F.y > 0.01 || F.z > 0.01) {
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(PrefilterTexture, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(BrdfTexture, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	// }

	vec3 ambient = (kD * diffuse + specular) * ao;

	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	fragColor = vec4(color, albedoRGBA.a);
}
