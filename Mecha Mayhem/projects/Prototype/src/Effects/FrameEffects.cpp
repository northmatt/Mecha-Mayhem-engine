#include "FrameEffects.h"
#include "Components/Sprite.h"

GBuffer* FrameEffects::baseEffect = nullptr;
IlluminationBuffer FrameEffects::illumBuffer;
TransparencyLayer FrameEffects::transparencyLayer;
Framebuffer* FrameEffects::shadowMap = nullptr;

FrameEffects::FrameEffects() { }

void FrameEffects::Init(unsigned width, unsigned height)
{
	PostEffect::Init("shaders/Post/passthrough_frag.glsl");

	//bloom shaders
	PostEffect::Init("shaders/Post/bloom_bright_pass.glsl");
	PostEffect::Init("shaders/Post/bloom_horizontal_blur.glsl");
	PostEffect::Init("shaders/Post/bloom_vertical_blur.glsl");
	PostEffect::Init("shaders/Post/bloom_combine_pass.glsl");

	//others
	PostEffect::Init("shaders/Post/color_correction_frag.glsl");
	PostEffect::Init("shaders/Post/greyscale_frag.glsl");
	PostEffect::Init("shaders/Post/sepia_frag.glsl");

	PostEffect::Init("shaders/Post/dof_extract.glsl");
	PostEffect::Init("shaders/Post/dof_deletion.glsl");
	PostEffect::Init("shaders/Post/dof_horizontal_blur.glsl");
	PostEffect::Init("shaders/Post/dof_vertical_blur.glsl");
	PostEffect::Init("shaders/Post/dof_combine.glsl");

	//gbuffer
	PostEffect::Init("shaders/Post/gBuffer_directional_frag.glsl");
	PostEffect::Init("shaders/Post/gBuffer_ambient_frag.glsl");

	//transparency
	PostEffect::Init("shaders/Post/transparency_frag.glsl");

	shadowMap = new Framebuffer();
	baseEffect = new GBuffer();

	//shadowMap->Unload();
	shadowMap->SetAsDepthBuffer(true);
	shadowMap->SetWrapMode(WrapMode::ClampToBorder);
	shadowMap->Init(shadowWidth, shadowHeight);

	baseEffect->Init(width, height);
	illumBuffer.Init(width, height);
	transparencyLayer.Init(width, height);
}

void FrameEffects::Unload()
{
	baseEffect->Unload();
	illumBuffer.Unload();
	transparencyLayer.Unload();
	shadowMap->Unload();

	delete baseEffect;
	delete shadowMap;
	PostEffect::UnloadShaders();
}

void FrameEffects::Init()
{
	RemoveAllEffects();

	illumBuffer.SetLightSpaceViewProj(m_shadowVP);
	illumBuffer.SetSun(m_sun);
}

void FrameEffects::Resize(unsigned width, unsigned height)
{
	baseEffect->Reshape(width, height);
	illumBuffer.Reshape(width, height);
	transparencyLayer.Reshape(width, height);

	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->Reshape(width, height);
	}
}

void FrameEffects::AddEffect(PostEffect* effect)
{
	layersOfEffects.push_back(effect);
}

void FrameEffects::InsertEffect(PostEffect* effect, int index)
{
	if (index <= size()) {
		layersOfEffects.insert(layersOfEffects.begin() + index, effect);
	}
}

void FrameEffects::RemoveEffect(int slot)
{
	if (layersOfEffects.size() <= slot || slot < 0)	return;

	if (layersOfEffects[slot] != nullptr) {

		layersOfEffects[slot]->Unload();
		delete layersOfEffects[slot];
		layersOfEffects[slot] = nullptr;
	}
	layersOfEffects.erase(layersOfEffects.begin() + slot);
}

void FrameEffects::Clear(bool paused)
{
	if (!paused)
		shadowMap->Clear();

	baseEffect->Clear();
	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->Clear();
	}
	//pauseEffect.Clear();
	
	//clear the illum buffer with white instead of clear colour
	glClearColor(1.f, 1.f, 1.f, 0.f);
	illumBuffer.Clear();
	transparencyLayer.Clear();
}

void FrameEffects::Bind()
{
	baseEffect->Bind();
}

void FrameEffects::UnBind()
{
	baseEffect->Unbind();
}

void FrameEffects::BindShadowMap()
{
	shadowMap->Bind();
	glDisable(GL_CULL_FACE);
}

void FrameEffects::UnBindShadowMap()
{
	shadowMap->Unbind();
	glEnable(GL_CULL_FACE);
}

void FrameEffects::BindTransparency()
{
	transparencyLayer.BindBuffer(1);
	glEnable(GL_BLEND);
}

void FrameEffects::UnBindTransparency()
{
	transparencyLayer.UnbindBuffer();
	glDisable(GL_BLEND);
}

void FrameEffects::Draw(/*bool paused*/)
{
	if (m_usingShadows)		shadowMap->BindDepthAsTexture(30);
	else		Sprite::m_textures[0].texture->Bind(30);

	illumBuffer.ApplyEffect(baseEffect);

	Texture2D::Unbind(30);

	transparencyLayer.ApplyEffect(&illumBuffer, baseEffect);

	PostEffect* prev = &transparencyLayer;

	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->ApplyEffect(prev);
		prev = layersOfEffects[i];
	}
	prev->DrawToScreen();
}

void FrameEffects::RemoveAllEffects()
{
	while (layersOfEffects.size()) {
		RemoveEffect(0);
		//RemoveEffect(layersOfEffects.size() - 1);
	}
}

void FrameEffects::SetShadowVP(const glm::mat4& VP)
{
	m_shadowVP = VP;
	illumBuffer.SetLightSpaceViewProj(m_shadowVP);
	illumBuffer.SetSun(m_sun);
}

void FrameEffects::SetShadowVP(float left, float right, float nearZ, float farZ, const glm::vec3& position)
{
	//Projection
	m_shadowVP = glm::ortho(left, right, left, right, nearZ, farZ) *
		//view rotation
		glm::lookAt(position + glm::vec3(m_sun._lightDirection), position, BLM::GLMup);
	illumBuffer.SetLightSpaceViewProj(m_shadowVP);
	illumBuffer.SetSun(m_sun);
}

FrameEffects& FrameEffects::Reattach()
{
	shadowMap->Clear();
	illumBuffer.SetLightSpaceViewProj(m_shadowVP);
	illumBuffer.SetSun(m_sun);

	return *this;
}
