#version 410
layout(location = 0) in vec3 inPosition;

uniform mat4 MVP;

void main() {
	// vertex position in light space
	gl_Position = MVP * vec4(inPosition, 1.0);
}


