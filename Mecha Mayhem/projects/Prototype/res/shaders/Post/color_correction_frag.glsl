#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;
layout (binding = 30) uniform sampler3D u_TexColorGrade;

const vec3 scale = vec3((64.0 - 1.0) / 64.0);
const vec3 offset = vec3(1.0 / (2.0 * 64.0));

uniform float u_Intensity;

void main() 
{
	vec4 source = texture(s_screenTex, inUV);

	frag_color.rgb = mix(source.rgb, frag_color.rgb = texture(u_TexColorGrade, scale * source.rgb + offset).rgb, u_Intensity);
	frag_color.a = source.a;
}