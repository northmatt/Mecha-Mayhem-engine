#version 410
#define MAX_LIGHTS 10
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

uniform vec3 colour;
uniform vec3 camPos;

uniform vec3  lightsPos[MAX_LIGHTS];
uniform vec3  lightsColour[MAX_LIGHTS];
uniform int lightCount;

uniform float ambientLightStrength;
uniform vec3  ambientColour;
uniform float ambientStrength;

uniform float specularStrength;
uniform float shininess;

uniform bool doAmbient;
uniform bool doDiffuse;
uniform bool doSpecular;

out vec4 frag_color;

void main() {
	//optimized for less garbage collection and memory mangement

	//spec = floor(spec * 5) * 0.2;
	//	if (spec < 0.25)
	//		spec = 0;
	//	else if (spec < 0.5)
	//		spec = 0.25;
	//	else if (spec < 0.75)
	//		spec = 0.5;
	//	else if (spec < 1)
	//		spec = 0.75;

	vec3 N = normalize(inNormal);
	vec3 camDir = normalize(camPos - inPos);
	vec3 lightDir = vec3(0.0, 0.0, 0.0);
	float lightDistSq = 0.0;
	vec3 total = vec3(0.0, 0.0, 0.0);
	
	float test = 0.0;

	for(int i = 0; i < lightCount; ++i) {
		lightDir = lightsPos[i] - inPos;
		lightDistSq = dot(lightDir, lightDir);
		lightDir = normalize(lightDir);
		test = 0.0;

		//diffuse
		if (doDiffuse)
			test += max(dot(N, lightDir), 0.0);

		//spec		SpecStrength															ShininessCoefficient
		if (doSpecular)
			test += specularStrength * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), shininess);

		total += (ambientLightStrength + test) / lightDistSq * lightsColour[i];
	}

	float testAmbientStrength = 0.0;
	if (doAmbient)
		testAmbientStrength = ambientStrength;
	else
		testAmbientStrength = 0.0;

	frag_color = vec4((ambientColour * testAmbientStrength + total) * colour, 1);
}