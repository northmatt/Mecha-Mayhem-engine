#version 410
layout(location = 4) in vec2 inUV;

uniform sampler2D s_texture;

out vec4 frag_color;

void main() {
	vec4 result = texture(s_texture, inUV);

	frag_color = result;
}