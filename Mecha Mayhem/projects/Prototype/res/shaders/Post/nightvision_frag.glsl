#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;

//Affects how greyscale
//Lower the number, closer we are to regular
uniform float u_Intensity = 1.0;

void main() 
{
	vec4 source = texture(s_screenTex, inUV);

	float luminence = 0.2989 * source.r + 0.587 * source.g + 0.114 * source.b;
	//source https://tobiasbu.wordpress.com/2016/01/16/opengl-night-vision-shader/

	vec3 grey = mix(source.rgb, luminence, u_Intensity);
	vec3 green = vec3(0.1, 0.95, 0.2);

	frag_color.rgb = grey * green

	//frag_color.rgb = source.rgb;
    frag_color.a = source.a;
}