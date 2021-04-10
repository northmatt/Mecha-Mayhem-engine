#version 420
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColour;
layout(location = 3) in vec3 inSpec;

uniform vec3 addColour;
uniform int receiveShadows;
uniform int rimLighting;

//multi render target
//we can render colour to all of these
layout(location = 0) out vec4 outColours;
layout(location = 1) out vec3 outNormals;
layout(location = 2) out vec4 outSpecs;
layout(location = 3) out vec3 outPositions;
layout(location = 4) out vec4 outEmissive;

void main() {
	//other classes store in material
	outSpecs.x = float(!bool(inSpec.x));
	outSpecs.y = inSpec.y;
	outSpecs.z = receiveShadows;
	outSpecs.w = rimLighting;

	//colours
	outColours.rgb = vec3(inColour + addColour) * outSpecs.x;
	outColours.a = inSpec.z;
	outEmissive = vec4(inColour + addColour, inSpec.z) * inSpec.x;
	
	//outputs normals as colour
	outNormals = (normalize(inNormal) * 0.5) + 0.5;

	//positions 
	outPositions = inPos;
}