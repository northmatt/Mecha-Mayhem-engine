#version 410
layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inPosition;

uniform sampler2D s_texture;
uniform int receiveShadows;
uniform vec3 addColour;

const float emissiveness = 0.0;
uniform float divide = 0.5;
const float shininess = 10.0;

//multi render target
//we can render colour to all of these
layout(location = 0) out vec4 outColours;
layout(location = 1) out vec3 outNormals;
layout(location = 2) out vec4 outSpecs;
layout(location = 3) out vec3 outPositions;
layout(location = 4) out vec4 outEmissive;

void main() {
	vec4 result = texture(s_texture, inUV);
	
	if (result.a < 0.5)
		discard;

	//other classes store in material
	outSpecs.x = float(!bool(emissiveness));
	outSpecs.y = shininess;
	outSpecs.z = receiveShadows;
	outSpecs.w = 0;		//no rim lighting

	outColours.rgb = vec3(result.rgb + addColour) * outSpecs.x * divide;
	outColours.a = 1.0;
	outEmissive = vec4(result.rgb + addColour, 1.0) * emissiveness;

	outNormals = (normalize(inNormal) * 0.5) + 0.5;

	outPositions = inPosition;
}