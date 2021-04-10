#pragma once
#include <GLM/gtc/matrix_transform.hpp>
#include "Post/TransparencyLayer.h"
#include "Utilities/BLM.h"

//aka the PP wrapper
class FrameEffects
{
public:
	FrameEffects();
	~FrameEffects() { RemoveAllEffects(); }

	//ensure you can't copy it, because of Framebuffer
	FrameEffects(const FrameEffects& other) = delete;
	FrameEffects(FrameEffects&& other) = delete;
	FrameEffects& operator=(const FrameEffects& other) = delete;
	FrameEffects& operator=(FrameEffects&& other) = delete;

	//for all effects (put static inits here)
	static void Init(unsigned width, unsigned height);
	//same as the init, but for unloading
	static void Unload();

	//change the buffers
	void Init();
	void Resize(unsigned width, unsigned height);

	//in order of added
	void AddEffect(PostEffect* effect);
	void InsertEffect(PostEffect* effect, int index);
	void RemoveEffect(int slot);

	//binds gBuffer
	static void Bind();
	static void UnBind();

	//binds shadow buffer
	static void BindShadowMap();
	static void UnBindShadowMap();

	//binds transparency buffer
	static void BindTransparency();
	static void UnBindTransparency();

	//for drawing purposes
	void Clear(bool paused);
	void Draw(/*bool paused*/);

	//removes all effects
	void RemoveAllEffects();

	//Get amount of effects
	size_t size() { return layersOfEffects.size(); }

	//get the effect at the index, and nullptr if invalid
	PostEffect* operator[](size_t index) {
		if (layersOfEffects.size() <= index)	return nullptr;
		return layersOfEffects[index];
	}

	static Framebuffer* GetDrawBuffer() {
		return &baseEffect->GetGBuffer();
	}

	static void SetCamPos(glm::vec3 pos, int camNum) {
		illumBuffer.SetCamPos(pos, camNum);
	}

	static void SetCamCount(int camNum) {
		illumBuffer.SetCamCount(camNum);
	}

	static void SetLights(std::array<glm::vec3, MAX_LIGHTS>& lightsPos,
						  std::array<glm::vec3, MAX_LIGHTS>& lightsColour, size_t LightCount) {
		illumBuffer.SendLights(lightsPos, lightsColour, LightCount);
	}

	void SetShadowVP(const glm::mat4& VP);
	void SetShadowVP(float left, float right, float nearZ, float farZ, const glm::vec3& position);

	glm::mat4 GetShadowVP() const { return m_shadowVP; }

	DirectionalLight& GetSun() {
		return m_sun;
	}

	FrameEffects& Reattach();

	void UsingShadows(bool choice) { m_usingShadows = choice; }
	bool GetUsingShadows() { return m_usingShadows; }

	static const unsigned shadowWidth	= 4096;//8192;
	static const unsigned shadowHeight	= 4096;//8192;
private:
	bool m_usingShadows = true;

	static GBuffer* baseEffect;
	static IlluminationBuffer illumBuffer;
	static TransparencyLayer transparencyLayer;
	static Framebuffer* shadowMap;

	std::vector<PostEffect*> layersOfEffects = {};

	DirectionalLight m_sun;

	glm::mat4 m_shadowVP =
		//Projection
		glm::ortho(-10.f, 10.f, -10.f, 10.f, 10.f, -10.f) *
		//View
		glm::lookAt(glm::vec3(m_sun._lightDirection), BLM::GLMzero, BLM::GLMup);
};