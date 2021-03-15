#version 420

//Data for the model
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColour;
layout(location = 3) in vec3 inSpec;
layout(location = 4) in vec2 inUV;

//The textures
uniform sampler2D s_texture;

//Multi render targer
layout(location = 0) out vec4 outColors;
layout(location = 1) out vec3 outNormals;
layout(location = 2) out vec3 outSpecs;
layout(location = 3) out vec3 outPositions;

void main() {
    outColors = vec4(inColour, 1.0) * texture(s_texture, inUV);

    outNormals = (normalize(inNormal) * 0.5) + 0.5;

    outSpecs = inSpec;

    outPositions = inPos;
}