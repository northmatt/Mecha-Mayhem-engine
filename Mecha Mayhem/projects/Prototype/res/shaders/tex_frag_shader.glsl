#version 410
#define MAX_LIGHTS 10
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColour;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inSpec;

uniform sampler2D s_texture;

uniform vec3 camPos;

uniform vec3  lightsPos[MAX_LIGHTS];
uniform vec3  lightsColour[MAX_LIGHTS];
uniform int lightCount;

uniform float ambientLightStrength;
uniform vec3  ambientColour;
uniform float ambientStrength;
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
	vec4 textureColour = texture(s_texture, inUV);
	if(u_noLight > 0)
	{
		frag_color = vec4(inColour * textureColour.rgb, textureColour.a * inSpec.z);
		return;
	}
	//optimized for less garbage collection and memory mangement
	vec3 N = normalize(inNormal);
	vec3 camDir = normalize(camPos - inPos);
	vec3 lightDir = vec3(0.0, 0.0, 0.0);
	vec3 total = vec3(0.0, 0.0, 0.0);

	for(int i = 0; i < lightCount; ++i) {
		lightDir = normalize(lightsPos[i] - inPos);
		if(u_ambient > 0)
		{
			total += (ambientLightStrength) * lightsColour[i]; 
		}
		if(u_diffuse > 0)
		{
		if(u_rampDiffuse > 0)
			total += texture(s_rampDiffuse , vec2(max(dot(N, lightDir), 0.0) / length(lightsPos[i] - inPos), 0.0)).g * lightsColour[i]; 
		else
			total += max(dot(N, lightDir), 0.0) / length(lightsPos[i] - inPos) * lightsColour[i]; 
		}
		if(u_specular > 0) 
		{
			if(u_rampSpec > 0)
			{
			total += texture(s_rampSpec, vec2(inSpec.x * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), inSpec.y), 0.0)).g * lightsColour[i];
			}
			else
			{
			total += inSpec.x * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), inSpec.y) * lightsColour[i];
			}
		}
	}
	if(u_customEff > 0)
	{
		total = sin(cos(tan(total + camPos) + camPos.yyy) + inPos.zxy);
	}
	if(u_ambient > 0)
	{
	frag_color = vec4((ambientColour * ambientStrength + total) * inColour * textureColour.rgb, textureColour.a * inSpec.z);
	}
	else
	{
	frag_color = vec4(total * inColour * textureColour.rgb, textureColour.a * inSpec.z);
	}
}