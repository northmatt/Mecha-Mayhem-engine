#pragma once
#include "GBuffer.h"
#include "Utilities/Lighting/UniformBuffer.h"
#include "Utilities/Lighting/PointLight.h"
#include "Utilities/Lighting/DirectionalLight.h"

enum Lights
{
	DIRECTIONAL = 1,
	AMBIENT
};

//This is a post effect to make our job easier
class IlluminationBuffer : public PostEffect
{
public:
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;
	
	//Makes it so apply effect with a PostEffect does nothing for this object
	void ApplyEffect(PostEffect* buffer) override { };
	//Can only apply effect using GBuffer object
	void ApplyEffect(GBuffer* gBuffer);

	//basically useless in our version
	//void DrawIllumBuffer();

	void SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj);
	void SetCamPos(glm::vec3 camPos, int camNum);
	void SetCamCount(int camNum);
	void SendLights(std::array<glm::vec3, MAX_LIGHTS>& lightsPos,
					std::array<glm::vec3, MAX_LIGHTS>& lightsColour, int LightCount) {
		_shaders[Lights::DIRECTIONAL]->SetUniform("lightsPos", *lightsPos.data(), LightCount);
		_shaders[Lights::DIRECTIONAL]->SetUniform("lightsColour", *lightsColour.data(), LightCount);
		_shaders[Lights::DIRECTIONAL]->SetUniform("lightCount", LightCount);
	}

	DirectionalLight& GetSunRef();
	
	//Sets the sun in the scene
	void SetSun(DirectionalLight newSun);
	void SetSun(glm::vec4 lightDir, glm::vec4 lightCol);

	void EnableSun(bool enabled);
	bool GetSunEnabled() const;

private:
	glm::mat4 _lightSpaceViewProj;
	std::array<glm::vec3, 4> _camPos = {
		glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0)
	};

	int _camCount = 1;

	bool _sunEnabled = true;

	UniformBuffer _sunBuffer;
	
	DirectionalLight _sun;
};