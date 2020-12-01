#version 410
#define MAX_LIGHTS 10
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

uniform vec3 camPos;

uniform vec3  lightsPos[MAX_LIGHTS];
uniform vec3  lightsColour[MAX_LIGHTS];
uniform int lightCount;

uniform float ambientLightStrength;
uniform vec3  ambientColour;
uniform float ambientStrength;

uniform vec3 colour;

uniform float specularStrength;
uniform float shininess;

out vec4 frag_color;

void main() {
	//optimized for less garbage collection and memory mangement
	vec3 N = normalize(inNormal);
	vec3 camDir = normalize(camPos - inPos);
	vec3 lightDir = vec3(0.0, 0.0, 0.0);
	vec3 total = vec3(0.0, 0.0, 0.0);

	for(int i = 0; i < lightCount; ++i) {
		lightDir = normalize(lightsPos[i] - inPos);

		//								 diffuse with attenuation									 SpecStrength															ShininessCoefficient
		total += (ambientLightStrength + max(dot(N, lightDir), 0.0) / length(lightsPos[i] - inPos) + specularStrength * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), shininess)) * lightsColour[i];
	}

	frag_color = vec4((ambientColour * ambientStrength + total) * colour, 1);
}