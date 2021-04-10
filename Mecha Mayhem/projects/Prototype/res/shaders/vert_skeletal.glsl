#version 410
#define MAX_JOINTS 26

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColour;
layout(location = 2) in vec3 inNormal;

// Skinned mesh input
layout(location = 3) in vec4 inJoint;
layout(location = 4) in vec4 inWeight;

uniform mat4 MVP;
uniform mat4 transform;

// Skinned mesh uniforms
uniform mat4[MAX_JOINTS] u_JointMatrices;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outColour;
layout(location = 3) out vec3 outSpec;

void main() {
	// Skeletal Animation
	mat4 skinMatrix = inWeight.x * u_JointMatrices[int(inJoint.x)] +
					  inWeight.y * u_JointMatrices[int(inJoint.y)] + 
					  inWeight.z * u_JointMatrices[int(inJoint.z)] +
					  inWeight.w * u_JointMatrices[int(inJoint.w)];
					  
	vec4 pos = skinMatrix * vec4(inPosition, 1.0);
	
	outPosition = (transform * pos).xyz;

	//gotta also rotate teh normals with the skin matrix
	outNormal = mat3(transform) * mat3(skinMatrix) * inNormal;

	outColour = inColour;

	//x = spec power, y = shininess, z = cast shadow
	outSpec = vec3(1, 2, 1);

	gl_Position = MVP * pos;
}


