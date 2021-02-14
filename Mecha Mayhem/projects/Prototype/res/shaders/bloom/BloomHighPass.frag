#version 410

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

uniform sampler2D s_screenTex;
uniform float uThreshold;

void main() 
{
	vec4 color = texture(s_screenTex, inUV);
	
	float luminance = (color.r + color.g + color.b) / 3.0;
	
	if (luminance > uThreshold) {
		frag_color = color;
	}
	else {
		frag_color = vec4(0.0, 0.0, 0.0, 1.0);
	}
}