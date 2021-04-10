#version 420
#define MAX_LIGHTS 10

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
	DirectionalLight sun;
};


layout(binding = 30) uniform sampler2D s_ShadowMap;

layout(binding = 0) uniform sampler2D s_albedoTex;
layout(binding = 1) uniform sampler2D s_normalsTex;
layout(binding = 2) uniform sampler2D s_specularTex;
layout(binding = 3) uniform sampler2D s_positionTex;
layout(binding = 4) uniform sampler2D s_emissiveTex;

uniform mat4 u_LightSpaceMatrix;
uniform vec3 u_camPos[4];
uniform int camCount;

uniform vec3  lightsPos[MAX_LIGHTS];
uniform vec3  lightsColour[MAX_LIGHTS];
uniform int lightCount;
const float specStrength = 1.0;

out vec4 frag_color;

const int softRange = 2;
const float texelSize = 1.0 / 4096;
const float area = 1.0 / ((softRange * 2 + 1) * (softRange * 2 + 1));

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
	//Perspective division
	vec3 projectionCoordinates = fragPosLightSpace.xyz / fragPosLightSpace.w;

	//Transform into a [0,1] range
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	//if (projectionCoordinates.z > 1.0)
	//	return 0.0;

	float currDepth = projectionCoordinates.z - bias;
	float shadow = 0.0;
	for (int x = -softRange; x <= softRange; ++x) {
		for (int y = -softRange; y <= softRange; ++y) {
			shadow += float(currDepth >
				texture(s_ShadowMap, projectionCoordinates.xy + vec2(x, y) * texelSize).r
				);
		}
	}
	return shadow * area;
	
	//Get the closest depth value from light's perspective (using our 0-1 range)
	//float closestDepth = texture(s_ShadowMap, projectionCoordinates.xy).r;

	//Get the current depth according to the light
	//float currentDepth = projectionCoordinates.z;

	//Check whether there's a shadow
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	//Return the value
	//return float(projectionCoordinates.z - bias > closestDepth);
	//return shadow;
}

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	//Normals
//	vec3 inNormal = texture(s_normalsTex, inUV).xyz;

//	if (length(inNormal) < 0.25) {
//		frag_color = vec4(1.0, 1.0, 1.0, 1.0);
//		return;
//	}

//	inNormal = (normalize(inNormal) * 2.0) - 1.0;
	vec3 inNormal = (normalize(texture(s_normalsTex, inUV).xyz) * 2.0) - 1.0;

	//Albedo
	vec4 textureColor = texture(s_albedoTex, inUV);

	//Specular
	vec3 texSpec = texture(s_specularTex, inUV).rgb;
	//Positions
	vec3 fragPos = texture(s_positionTex, inUV).rgb;

	// Diffuse
	vec3 lightDir = normalize(-sun._lightDirection.xyz);
	float dif = max(dot(inNormal, lightDir), 0.0);
	vec3 diffuse = dif * sun._lightCol.rgb;// add diffuse intensity

	int camNum = int(camCount > 1) * int(inUV.x > 0.5) + int(camCount > 2) * int(inUV.y < 0.5) * 2;

	// Specular
	vec3 viewDir  = normalize(u_camPos[camNum] - fragPos);
	vec3 h        = normalize(lightDir + viewDir);


	// Get the specular from the specular map
	float spec = pow(max(dot(inNormal, h), 0.0), texSpec.y); // Shininess coefficient (can be a uniform)
	vec3 specular = sun._lightSpecularPow * spec * sun._lightCol.rgb; // Can also use a specular color

	// Get the albedo from the diffuse / albedo map	

	//Lighting
	vec4 inFragPosLightSpace = u_LightSpaceMatrix * vec4(fragPos, 1.0);

	//float lightNorm = dot(inNormal, lightDir);
	//float bias = lightNorm > 0 ? 0 : max(sun._shadowBiasMax * (1.0 - lightNorm), sun._shadowBiasMin); 
	float bias = max(sun._shadowBiasMax * (1.0 - dot(inNormal, lightDir)), sun._shadowBiasMin); 
	float shadow = texSpec.z * ShadowCalculation(inFragPosLightSpace, bias);

	vec3 result = (
		(sun._ambientPow * sun._ambientCol.rgb) + // global ambient light
		(1.0 - shadow) * //Shadow value
		(diffuse + specular) // light factors from our single light
		) * texSpec.x; // don't render when emissive


	float lightDistSq = 0.0;
    for(int i = 0; i < lightCount; ++i) {
		lightDir = lightsPos[i] - fragPos;
		lightDistSq = dot(lightDir, lightDir);
		lightDir = normalize(lightDir);

		//			diffuse
		result += ((max(dot(inNormal, lightDir), 0.0)
			//	SpecStrength															ShininessCoefficient
			+ specStrength * pow(max(dot(inNormal, normalize(viewDir + lightDir)), 0.0), texSpec.y))
			//	attenuation	emissive removal
			/ lightDistSq * texSpec.x) * lightsColour[i];
	}

		result += texture(s_emissiveTex, inUV).rgb;

	if (textureColor.a < 0.31)
		result = vec3(1.0, 1.0, 1.0);

	frag_color = vec4(result, 1.0);
}
