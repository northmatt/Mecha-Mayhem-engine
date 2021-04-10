#version 420
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

uniform vec3 colour;
uniform vec3 addColour;

uniform float emissiveness;
uniform float shininess;
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
	outSpecs.x = float(!bool(emissiveness));
	outSpecs.y = shininess;
	outSpecs.z = receiveShadows;
	outSpecs.w = rimLighting;

	//colours
	outColours.rgb = vec3(colour + addColour) * outSpecs.x;
	outColours.a = 1.0;
	outEmissive = vec4(colour + addColour, 1.0) * emissiveness;
	
	//outputs normals as colour
	outNormals = (normalize(inNormal) * 0.5) + 0.5;

	//positions 
	outPositions = inPos;
}