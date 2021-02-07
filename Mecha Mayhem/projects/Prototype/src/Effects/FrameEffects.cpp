#include "FrameEffects.h"
#include "Engine/BackEnd.h"

FrameEffects::FrameEffects() { }

void FrameEffects::Init(unsigned width, unsigned height)
{
	while (layersOfEffects.size() > 0) {
		RemoveEffect(0);
	}

	baseEffect.Init(width, height);
}

void FrameEffects::Resize(unsigned width, unsigned height)
{
	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->Reshape(width, height);
	}

	baseEffect.Reshape(width, height);
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
}

void FrameEffects::Bind()
{
	baseEffect.BindBuffer(0);
}

void FrameEffects::UnBind()
{
	baseEffect.UnbindBuffer();
}

void FrameEffects::Draw()
{
	PostEffect* prev = &baseEffect;
	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->ApplyEffect(prev);
		prev = layersOfEffects[i];
	}
	glViewport(0, 0, BackEnd::GetHalfWidth() * 2, BackEnd::GetHalfHeight() * 2);
	prev->DrawToScreen();
}
