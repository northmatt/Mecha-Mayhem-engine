#version 410
#define MAX_LIGHTS 10
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColour;
layout(location = 3) in vec3 inNormal;

uniform vec3 camPos;

uniform vec3  lightsPos[MAX_LIGHTS];
uniform vec3  lightsColour[MAX_LIGHTS];
uniform int lightCount;
uniform bool unLit;

uniform float ambientLightStrength;
uniform vec3  ambientColour;
uniform float ambientStrength;

uniform vec3 colour;

uniform float specularStrength;
uniform float shininess;

out vec4 frag_color;

void main() {
	//optimized for less garbage collection and memory mangement
	vec3 result = vec3(0.0, 0.0, 0.0);

	if (unLit)
		result = inColour * colour;
	else {
		vec3 N = normalize(inNormal);
		vec3 camDir = normalize(camPos - inPos);
		vec3 lightDir = vec3(0.0, 0.0, 0.0);
		vec3 total = vec3(0.0, 0.0, 0.0);

		for(int i = 0; i < lightCount; ++i) {
			lightDir = normalize(lightsPos[i] - inPos);

			//Add diffuse intensity with attenuation
			float diffuse = max(dot(N, lightDir), 0.0) / length(lightsPos[i] - inPos);

			//Specular		SpecStrength															ShininessCoefficient
			float specular = specularStrength * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), shininess);

			total += (ambientLightStrength + diffuse + specular) * lightsColour[i];
		}

		result = (ambientColour * ambientStrength + total) * inColour * colour;
	}

	frag_color = vec4(result, 1);
}