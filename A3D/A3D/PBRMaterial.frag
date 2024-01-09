#version 330 core

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 4
#endif

out vec4 fragColor;

in VS_OUT {
	vec3 WorldPos;
	vec2 TexCoord;
	vec3 Normal;
} fsIn;

layout (std140) uniform SceneUBO_Data {
	vec4 cameraPos;
	
	vec4 lightsPos[MAX_LIGHTS];
	vec4 lightsColor[MAX_LIGHTS];
};

uniform sampler2D AlbedoTexture;
uniform sampler2D NormalTexture;
uniform sampler2D MetallicTexture;
uniform sampler2D RoughnessTexture;
uniform sampler2D AOTexture;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(NormalTexture, fsIn.TexCoord).xyz * 2.0 - 1.0;
	
	vec3 Q1 = dFdx(fsIn.WorldPos);
	vec3 Q2 = dFdy(fsIn.WorldPos);
	vec3 st1 = dFdx(fsIn.TexCoord);
	vec3 st2 = dFdx(fsIn.TexCoord);
	
	vec3 N = normalize(fsIn.Normal);
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

void main() {
	vec3 albedo = pow(texture(AlbedoTexture, fsIn.TexCoord).rgb, vec3(2.2));
	float metallic = texture(MetallicTexture, fsIn.TexCoord).r;
	float roughness = texture(RoughnessTexture, fsIn.TexCoord).r;
	float ao = texture(AOTexture, fsIn.TexCoord).r;
	
	vec3 N = getNormalFromMap();
	vec3 V = normalize(cameraPos - fsIn.WorldPos);
	
	vec3 F0 = mix(vec3(0.04), albedo, metallic);
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < MAX_LIGHTS; ++i)
	{
		vec3 L = normalize(lightsPos[i] - fsIn.WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(lightsPos[i] - fsIn.WorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightsColor[i] * attenuation;
		
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
		
		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;
		
		vec3 kS = F;
	}
	
	vec4 diffuseMatColor = vec4(0, 0, 1, 1);
	vec4 ambientMatColor = vec4(0, 1, 0, 1);
	vec4 specularMatColor = vec4(1, 0, 0, 1);
	
	vec3 lightPos = vec3(0, 30, 0);
	vec3 eyePos = vec3(0, 0, 0);
	float specularExponent = 32.0;
	
	float diffuseStrength = 0;
	float ambientStrength = 0.05;
	float specularStrength = 0;
	
	// Calculate lighting strengths
	{
		vec3 lightDir = normalize(lightPos - fsIn.Pos);
		vec3 normal = normalize(fsIn.Normal);
		
		// Diffuse
		diffuseStrength = max(dot(lightDir, normal), 0.0);
		
		// Specular
		vec3 viewDir = normalize(eyePos - fsIn.Pos);
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		
		specularStrength = pow(max(dot(normal, halfwayDir), 0.0), specularExponent);
	}
	
	vec4 diffuseColor = texture(DiffuseTexture, fsIn.TexCoord) * diffuseMatColor;
	vec4 ambientColor = texture(AmbientTexture, fsIn.TexCoord) * ambientMatColor;
	vec4 specularColor = texture(SpecularTexture, fsIn.TexCoord) * specularMatColor;
	
	fragColor = vec4(
		(diffuseColor.rgb * diffuseStrength)
		+ (ambientColor.rgb * ambientStrength)
		+ (specularColor.rgb * specularStrength)
	, 1.0);
}
