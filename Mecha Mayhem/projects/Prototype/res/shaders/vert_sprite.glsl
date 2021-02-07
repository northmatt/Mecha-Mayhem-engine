#version 410
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

uniform mat4 MVP;

layout(location = 0) out vec2 outUV;

void main() {
	outUV = inUV;

	gl_Position = MVP * vec4(inPosition, 1.0);
}


