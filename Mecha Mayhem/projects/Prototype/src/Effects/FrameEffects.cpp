#include "FrameEffects.h"

FrameEffects::FrameEffects() { }

void FrameEffects::Init()
{
	PostEffect::Init("shaders/Post/passthrough_frag.glsl");

	//bloom shaders
	PostEffect::Init("shaders/Post/bloom_bright_pass.glsl");
	PostEffect::Init("shaders/Post/bloom_horizontal_blur.glsl");
	PostEffect::Init("shaders/Post/bloom_vertical_blur.glsl");
	PostEffect::Init("shaders/Post/bloom_combine_pass.glsl");

	//others
	//PostEffect::Init("shaders/Post/color_correction_frag.glsl");
	PostEffect::Init("shaders/Post/greyscale_frag.glsl");
	PostEffect::Init("shaders/Post/sepia_frag.glsl");
	PostEffect::Init("shaders/Post/toon_frag.glsl");

	PostEffect::Init("shaders/gBuffer_directional_frag.glsl");
	PostEffect::Init("shaders/gBuffer_point_frag.glsl");
	PostEffect::Init("shaders/gBuffer_ambient_frag.glsl");
}

void FrameEffects::Unload()
{
	PostEffect::UnloadShaders();
}

void FrameEffects::Init(unsigned width, unsigned height)
{
	RemoveAllEffects();

	baseEffect.Init(width, height);
	lighting.Init(width, height);
}

void FrameEffects::Resize(unsigned width, unsigned height)
{
	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->Reshape(width, height);
	}

	baseEffect.Reshape(width, height);
	lighting.Reshape(width, height);
}

void FrameEffects::AddEffect(PostEffect* effect)
{
	layersOfEffects.push_back(effect);
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

void FrameEffects::Clear()
{
	baseEffect.Clear();
	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->Clear();
	}
	lighting.Clear();
	//pauseEffect.Clear();
}

void FrameEffects::Bind()
{
	baseEffect.Bind();
}

void FrameEffects::UnBind()
{
	baseEffect.Unbind();
}

void FrameEffects::Draw(/*bool paused*/)
{
	lighting.ApplyEffect(&baseEffect);

	PostEffect* prev = &lighting;
	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->ApplyEffect(prev);
		prev = layersOfEffects[i];
	}
	/*if (paused) {
		pauseEffect->ApplyEffect(prev);
		prev = &pauseEffect;
	}*/
	prev->DrawToScreen();
}

void FrameEffects::RemoveAllEffects()
{
	while (layersOfEffects.size()) {
		RemoveEffect(0);
	}
}
