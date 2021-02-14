#version 410

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

uniform sampler2D s_screenTex;

void main() {
	vec4 source = texture(s_screenTex, inUV);

	frag_color.rgb = source.rgb;
	frag_color.a = source.a;
}