#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;
layout (binding = 1) uniform sampler2D s_depthTex;

uniform float depthLimit;

void main() 
{
	float currDepth = texture(s_depthTex, inUV).r;

	if (currDepth > depthLimit)
		discard;

	frag_color = texture(s_screenTex, inUV);

}