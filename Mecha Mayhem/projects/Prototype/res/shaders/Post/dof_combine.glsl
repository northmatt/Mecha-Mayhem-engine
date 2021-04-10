#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_nearTex;
layout (binding = 1) uniform sampler2D s_farTex;

void main() 
{
	vec4 near = texture(s_nearTex, inUV);

	frag_color = near.a < 1 ? texture(s_farTex, inUV) : near;
}