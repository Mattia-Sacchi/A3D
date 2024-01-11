#version 330 core

out vec4 fragColor;

in VS_OUT {
	vec3 Pos;
	vec2 TexCoord;
	vec3 Normal;
} fsIn;

uniform sampler2D DiffuseTexture;
uniform sampler2D AmbientTexture;
uniform sampler2D SpecularTexture;

void main() {
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
