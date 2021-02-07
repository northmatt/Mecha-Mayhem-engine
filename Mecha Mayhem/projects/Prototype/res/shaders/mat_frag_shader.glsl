#version 410
#define MAX_LIGHTS 10
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColour;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inSpec;

uniform vec3 camPos;

uniform vec3  lightsPos[MAX_LIGHTS];
uniform vec3  lightsColour[MAX_LIGHTS];
uniform int lightCount;

uniform float ambientLightStrength;
uniform vec3  ambientColour;
uniform float ambientStrength;

uniform bool usingAmbient;
uniform bool usingSpecular;
uniform bool usingDiffuse;
uniform bool noLighting;
uniform bool toonShading;

uniform bool usingDiffuseRamp;
uniform bool usingSpecularRamp;

uniform sampler2D diffuseRamp;
uniform sampler2D specularRamp;

const float bands = 5.0;
const float fix = 1.0 / bands;

out vec4 frag_color;

void main() {

	if(noLighting)
	{	
		frag_color = vec4(inColour, inSpec.z);
		return;
	}
	
	//optimized for less garbage collection and memory mangement
	vec3 N = normalize(inNormal);
	vec3 camDir = normalize(camPos - inPos);
	vec3 lightDir = vec3(0.0, 0.0, 0.0);
	vec3 total = vec3(0.0, 0.0, 0.0);
	float hold = 0;

	for(int i = 0; i < lightCount; ++i) {
		lightDir = normalize(lightsPos[i] - inPos);
		hold = 0;

		if(usingAmbient)
			hold += ambientLightStrength;

		if(usingDiffuse)
		{
			if(usingDiffuseRamp)
				hold += texture(diffuseRamp, vec2(max(dot(N, lightDir), 0.0) / length(lightsPos[i] - inPos), 0.0)).r;
			else
				hold += max(dot(N, lightDir), 0.0) / length(lightsPos[i] - inPos);
		}
		if(usingSpecular)
		{
			if(usingSpecularRamp)
				hold += texture(specularRamp, vec2(inSpec.x * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), inSpec.y), 0.0)).r;
			else
				hold += inSpec.x * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), inSpec.y);
		}
		//toon shading here
		if(toonShading)
			hold = floor(hold * bands) * fix;

		total += hold * lightsColour[i];
	}
	
	if (usingAmbient)
		frag_color = vec4(total * inColour, inSpec.z);
	else
		frag_color = vec4((ambientColour * ambientStrength + total) * inColour, inSpec.z);
}