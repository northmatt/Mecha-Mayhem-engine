#version 410
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inUV;

uniform mat4 MVP;
uniform mat4 model;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outPosition;

const vec3 DefaultNormal = vec3(0, 0, 1);

void main() {
	gl_Position = MVP * vec4(inPosition, 1.0);

	outUV = inUV.xy;

	outNormal = mat3(model) * DefaultNormal;

	outPosition = (model * vec4(inPosition, 1.0)).xyz;
}


