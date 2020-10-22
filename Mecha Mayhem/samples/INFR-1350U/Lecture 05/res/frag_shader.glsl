#version 410

layout(location = 1) in vec3 inColor;
// Lecture 5
layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec3 inNormal;


out vec4 frag_color;

// Lecture 5
uniform vec3 LightPos;

void main() {
	
	// Lecture 5
	vec3 lightColour = vec3(1.0, 1.0, 1.0);

	float ambientStrength = 0.05;
	vec3 ambient = ambientStrength * lightColour * inColor;

	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(LightPos - inPosition);
	float D = max(dot(N, lightDir), 0.0);
	vec3 diffuse = D * inColor;			//can add diffuse density

	//attenuation
	float dist = length(LightPos - inPosition);
	diffuse = diffuse / dist;

	vec3 camPos = vec3(0.0, 0.0, 3.0);	//Pass this as a uniform from main next time
	float specularStrength = 1.0;		//should also be a uniform
	vec3 camDir = normalize(camPos - inPosition);
	//Phong Model
	/*
	vec3 reflectDir = reflect(-lightDir, N);
	float S = pow(max(dot(camDir, reflectDir), 0.0), 16); //shininess coefficient (can be uniform)
	*/
	//Blinn-Phong Model
	vec3 H = normalize(lightDir + camDir);	//halfway from lightDir and CamDir
	float S = pow(max(dot(H, N), 0.0), 128);

	vec3 specular = specularStrength * S * lightColour;			//can use specular colours as well

	vec3 result = (ambient + diffuse + specular);

	frag_color = vec4(result, 1.0);
}