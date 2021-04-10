#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;

uniform float u_TexelSize;

//done with the guidance of https://learnopengl.com/Advanced-Lighting/Bloom and spritelib bloom shaders
//also https://pingpoli.medium.com/the-bloom-post-processing-effect-9352fa800caf for the spread adjustability

vec4 GetColour(float offset) {
	vec4 texCol = texture(s_screenTex, vec2(inUV.x, inUV.y + offset));
	
	return float(texCol.a > 0.31) * vec4(texCol.rgb, 1.0);
}

void main() 
{
	frag_color = GetColour(0);
	
	frag_color += GetColour(u_TexelSize * -4);
	frag_color += GetColour(u_TexelSize * -3);
	frag_color += GetColour(u_TexelSize * -2);
	frag_color += GetColour(u_TexelSize * -1);
	frag_color += GetColour(u_TexelSize * 1);
	frag_color += GetColour(u_TexelSize * 2);
	frag_color += GetColour(u_TexelSize * 3);
	frag_color += GetColour(u_TexelSize * 4);

	frag_color /= frag_color.a;
}