#version 410
layout(location = 0) in vec3 inPos1;
layout(location = 1) in vec3 inPos2;

uniform float t;
uniform mat4 MVP;

void main() {
	// vertex position in light space
	gl_Position = MVP * vec4(mix(inPos1, inPos2, t), 1);
}


