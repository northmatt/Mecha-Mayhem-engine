#version 410
layout(location = 0) in vec3 inPosition;

uniform mat4 lightVPMatrix;
uniform mat4 model;

void main() {
	// vertex position in light space
	gl_Position = lightVPMatrix * model * vec4(inPosition, 1.0);
}


