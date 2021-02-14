#version 410
layout(location = 0) in vec2 inUV;

uniform bool doTex;

uniform sampler2D s_texture;

out vec4 frag_color;

void main() {
	vec4 result;

	if (doTex)
		result = texture(s_texture, inUV);
	else
		result = vec4(0.1,0.1,0.1,1.0);

	frag_color = result;
}