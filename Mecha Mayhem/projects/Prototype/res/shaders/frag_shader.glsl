#version 410
#define MAX_LIGHTS 10
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

uniform vec3 colour;
uniform vec3 camPos;

uniform vec3  lightsPos[MAX_LIGHTS];
uniform vec3  lightsColour[MAX_LIGHTS];
uniform int lightCount;
//uniform bool unLit;

uniform float ambientLightStrength;
uniform vec3  ambientColour;
uniform float ambientStrength;

uniform float specularStrength;
uniform float shininess;

//general
uniform int u_noLight;
uniform int u_ambient;
uniform int u_diffuse;
uniform int u_specular;
uniform int u_customEff;
//ramp
uniform int u_rampDiffuse;
uniform int u_rampSpec;
uniform sampler2D s_rampDiffuse;
uniform sampler2D s_rampSpec;

out vec4 frag_color;

void main() {
	//optimized for less garbage collection and memory mangement
	vec3 result = vec3(0.0, 0.0, 0.0);

	//spec = floor(spec * 5) * 0.2;
	//	if (spec < 0.25)
	//		spec = 0;
	//	else if (spec < 0.5)
	//		spec = 0.25;
	//	else if (spec < 0.75)
	//		spec = 0.5;
	//	else if (spec < 1)
	//		spec = 0.75;

	if (u_noLight > 0)
		result = colour;
	else {
		vec3 N = normalize(inNormal);
		vec3 camDir = normalize(camPos - inPos);
		vec3 lightDir = vec3(0.0, 0.0, 0.0);
		vec3 total = vec3(0.0, 0.0, 0.0);

		for(int i = 0; i < lightCount; ++i) {
			lightDir = normalize(lightsPos[i] - inPos);

			//Add diffuse intensity with attenuation
			float diffuse = max(dot(N, lightDir), 0.0) / length(lightsPos[i] - inPos) * u_diffuse;

			if(u_rampDiffuse > 0)
			{
				diffuse = texture(s_rampDiffuse, vec2(diffuse, 0.0)).g;
			}

			//Specular		SpecStrength													ShininessCoefficient
			float specular = specularStrength * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), shininess) * u_specular;

			if(u_rampSpec > 0)
			{
				specular = texture(s_rampSpec, vec2(specular, 0.0)).g;
			}

			total += ((ambientLightStrength * u_ambient) + diffuse + specular) * lightsColour[i];
		}
		if(u_customEff > 0)
		{
		total = sin(cos(tan(total + camPos) + camPos.yyy) + inPos.zxy);
		}
		result = ((ambientColour * ambientStrength * u_ambient) + total) * colour;
	}

	frag_color = vec4(result, 1);
}