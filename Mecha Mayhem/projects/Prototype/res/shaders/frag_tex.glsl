#version 410
#define MAX_LIGHTS 10
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColour;
layout(location = 3) in vec3 inSpec;
layout(location = 4) in vec2 inUV;

uniform vec3 camPos;

uniform vec3  lightsPos[MAX_LIGHTS];
uniform vec3  lightsColour[MAX_LIGHTS];
uniform int lightCount;

uniform float ambientLightStrength;
uniform vec3  ambientColour;
uniform float ambientStrength;

uniform bool doAmbient;
uniform bool doDiffuse;
uniform bool doSpecular;
uniform bool doTex;

uniform sampler2D s_texture;

out vec4 frag_color;

void main() {
	//optimized for less garbage collection and memory mangement
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

		//spec		SpecStrength													 ShininessCoefficient
		if (doSpecular)
			test += inSpec.x * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), inSpec.y);

		total += (ambientLightStrength + test) / lightDistSq * lightsColour[i];
	}

	vec4 textureColour;
	if (doTex)
		textureColour = texture(s_texture, inUV);
	else
		textureColour = vec4(0.1,0.1,0.1,1.0);

	float testAmbientStrength = 0.0;
	if (doAmbient)
		testAmbientStrength = ambientStrength;
	else
		testAmbientStrength = 0.0;

	frag_color = vec4((ambientColour * testAmbientStrength + total) * inColour * textureColour.rgb, textureColour.a * inSpec.z);
}