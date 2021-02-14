#version 410

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

uniform sampler2D s_screenTex; //Source image
uniform float uPixelSize; //1.0 / Window_Width

void main() 
{
	//Sample pixels in a horizontal row
	//Weight should add up to 1
	frag_color = vec4(0.0, 0.0, 0.0, 0.0);

	frag_color += texture(s_screenTex, vec2(inUV.x - 4.0 *	uPixelSize, inUV.y)) * 0.06;
	frag_color += texture(s_screenTex, vec2(inUV.x - 3.0 *	uPixelSize, inUV.y)) * 0.09;
	frag_color += texture(s_screenTex, vec2(inUV.x - 2.0 *	uPixelSize, inUV.y)) * 0.12;
	frag_color += texture(s_screenTex, vec2(inUV.x -		uPixelSize, inUV.y)) * 0.15;
	frag_color += texture(s_screenTex, vec2(inUV.x,						inUV.y)) * 0.16;
	frag_color += texture(s_screenTex, vec2(inUV.x + 		uPixelSize,	inUV.y)) * 0.15;
	frag_color += texture(s_screenTex, vec2(inUV.x + 2.0 *	uPixelSize,	inUV.y)) * 0.12;
	frag_color += texture(s_screenTex, vec2(inUV.x + 3.0 *	uPixelSize,	inUV.y)) * 0.09;
	frag_color += texture(s_screenTex, vec2(inUV.x + 4.0 *	uPixelSize,	inUV.y)) * 0.06;
}