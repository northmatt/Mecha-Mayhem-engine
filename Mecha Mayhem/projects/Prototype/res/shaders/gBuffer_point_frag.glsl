#version 420

layout(location = 0) in vec2 inUV;

struct PointLight {
	vec4 _lightPos;
	vec4 _lightCol;

	//FLOATS
	float _lightSpecularPow;
};

layout (std140, binding = 0) uniform u_Lights {
	PointLight light;
};

layout (binding = 30) uniform sampler2D s_ShadowMap;

layout(binding = 0) uniform sampler2D s_albedoTex;
layout(binding = 1) uniform sampler2D s_normalsTex;
layout(binding = 2) uniform sampler2D s_specularTex;
layout(binding = 3) uniform sampler2D s_positionTex;
layout(binding = 4) uniform sampler2D s_lightAccumTex;

uniform mat4 u_LightSpaceMatrix;
uniform vec3 u_CamPos;
uniform vec3 u_ambientLightPow;
uniform vec3 u_ambientCol;
uniform vec3 u_ambientPow;

out vec4 frag_color;

void main() {
    vec4 textureColor = texture(s_albedoTex, inUV);
    vec3 inNormal = (normalize(texture(s_normalsTex, inUV).rgb) * 2.0) - 1.0;
    float texSpec = texture(s_specularTex, inUV).r;
    vec3 fragPos = texture(s_positionTex, inUV).rgb;

	vec3 N = normalize(inNormal);
	vec3 camDir = normalize(u_CamPos - fragPos);
	vec3 lightDir = vec3(0.0, 0.0, 0.0);
	float lightDistSq = 0.0;
	vec3 total = vec3(0.0, 0.0, 0.0);

	lightDir = light._lightPos.xyz - fragPos;
	lightDistSq = dot(lightDir, lightDir);
	lightDir = normalize(lightDir);

	//								 diffuse				   SpecStrength													 ShininessCoefficient
	total = (u_ambientLightPow + max(dot(N, lightDir), 0.0) + texSpec * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), 4.0)) / lightDistSq * light._lightCol.rgb;

	vec3 result = vec3(0.0, 0.0, 0.0);
        
    if(textureColor.a < 0.31) {
        result = vec3(1.0, 1.0, 1.0);
    } else {
		result = u_ambientCol * u_ambientPow + total;
	}

	frag_color = vec4(result, 1.0);
}