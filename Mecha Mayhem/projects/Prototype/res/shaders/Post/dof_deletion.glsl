#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;
layout (binding = 1) uniform sampler2D s_extracted;

void main() 
{
	float test = texture(s_extracted, inUV).a;

	if (test > 0.9)
		discard;

	frag_color = texture(s_screenTex, inUV);

}