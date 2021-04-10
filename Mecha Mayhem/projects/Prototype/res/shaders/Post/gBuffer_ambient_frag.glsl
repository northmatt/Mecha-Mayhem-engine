#version 420

layout(location = 0) in vec2 inUV;

struct DirectionalLight
{
	//Light direction (defaults to down, to the left, and a little forward)
	vec4 _lightDirection;

	//Generic Light controls
	vec4 _lightCol;

	//Ambience controls
	vec4 _ambientCol;
	float _ambientPow;
	
	//Power controls
	float _lightAmbientPow;
	float _lightSpecularPow;
	
	float _shadowBiasMax;
	float _shadowBiasMin;
};

layout (std140, binding = 0) uniform u_Lights
{
	DirectionalLight light;
};

layout(binding = 0) uniform sampler2D s_albedoTex;
layout(binding = 1) uniform sampler2D s_normalsTex;
layout(binding = 2) uniform sampler2D s_specTex;
layout(binding = 3) uniform sampler2D s_positionTex;
layout(binding = 4) uniform sampler2D s_emissiveTex;
layout(binding = 5) uniform sampler2D s_lightAccumTex;

//uniform vec3 for clear colour?
uniform vec3 u_camPos[4];
uniform int camCount;

const vec3 rimColour = vec3(5, 5, 5);

out vec4 frag_color;

void main()
{
	vec4 textureColour = texture(s_albedoTex, inUV);
	vec4 emissiveColour = texture(s_emissiveTex, inUV);
	//lights
	vec3 lightAccum = texture(s_lightAccumTex, inUV).rgb;
	
	//rim lighting based on https://www.youtube.com/watch?v=hB_F3KwMCkU
	//vec3 inPosition = texture(s_positionTex, inUV).xyz;
	//vec3 inNormal = (normalize(texture(s_normalsTex, inUV).xyz) * 2.0) - 1.0;
	int camNum = int(camCount > 1) * int(inUV.x > 0.5) + int(camCount > 2) * int(inUV.y < 0.5) * 2;

	//float rimLight = 1.0 - dot(normalize(u_camPos[camNum] - inPosition), inNormal);
	//rimLight = clamp(rimLight - 0.5, 0.0, 1.0);
	//add rim lighting
	lightAccum += (clamp(
		0.5 - dot(
			normalize(u_camPos[camNum] - texture(s_positionTex, inUV).xyz),
			(normalize(texture(s_normalsTex, inUV).xyz) * 2.0) - 1.0
		), 0.0, 1.0) * float(texture(s_specTex, inUV).w > 0.5)) * rimColour;

	//Ambient math
	vec3 ambient = light._lightAmbientPow * light._ambientCol.rgb;

	//result of all lighting
	vec3 result = (ambient + lightAccum) * (textureColour.rgb + emissiveColour.rgb * float(emissiveColour.a > 0.5));

	//Add clear colour?
	//result = result * skybox.rgb;

	//light accumulation
	frag_color = vec4(result, 1.0);
}