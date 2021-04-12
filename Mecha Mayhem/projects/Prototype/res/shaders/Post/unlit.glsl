#version 420

layout(location = 0) in vec2 inUV;

uniform vec3 colour;

out vec4 frag_color;

void main() {
	frag_color = vec4(colour,1);
}