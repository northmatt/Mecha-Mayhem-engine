#version 410
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 MVP;
uniform mat4 transform;
uniform vec3 colour;
uniform float specularStrength;
uniform float shininess;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outColour;
layout(location = 3) out vec3 outSpecStrength;
layout(location = 4) out vec2 outUV;

void main() {
	// vertex position in clip space
	gl_Position = MVP * vec4(inPosition, 1.0);

	outPosition = (transform * vec4(inPosition, 1)).xyz;
	outNormal = mat3(transform) * inNormal;
	outColour = colour;
	outSpecStrength = vec3(specularStrength, shininess, 1.0);
	outUV = vec2(0,0);
}


