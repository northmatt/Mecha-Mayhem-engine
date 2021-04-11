#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;
layout (binding = 1) uniform sampler2D s_overlayTex;
layout (binding = 2) uniform sampler2D s_screenDepth;
layout (binding = 3) uniform sampler2D s_overlayDepth;

void main() 
{
	float sourceDepth = texture(s_screenDepth, inUV).r;
	float transparencyDepth = texture(s_overlayDepth, inUV).r;

	vec4 source = texture(s_screenTex, inUV);
	vec4 transparency = texture(s_overlayTex, inUV);

	//first check if it's front / should render (1 or 0 / on or off)
	float transparencyAmt = float(transparencyDepth < sourceDepth) * transparency.a;

	//					src * src_alpha					+		dest * (1 - src_alpha)
	frag_color = vec4(transparency.rgb * transparencyAmt + source.rgb * (1 - transparencyAmt), 1.0);
}