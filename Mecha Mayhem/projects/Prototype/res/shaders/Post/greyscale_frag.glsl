#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;

uniform float time;

//Affects how greyscale
//Lower the number, closer we are to regular
uniform float u_Intensity = 1.0;

//https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83 function used
float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
	float fl = floor(p);
  float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}
void main() 
{
	vec4 source = texture(s_screenTex, inUV);

	float luminence = 0.2989 * source.r + 0.587 * source.g + 0.114 * source.b;

	vec3 grey = mix(source.rgb, vec3(luminence), u_Intensity);

	frag_color.rgb = grey * noise(sin(time));

	//frag_color.rgb = source.rgb;
    frag_color.a = source.a;
}